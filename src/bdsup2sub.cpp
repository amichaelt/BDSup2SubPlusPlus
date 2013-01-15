/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
 * and Copyright 2012 Miklos Juhasz (mjuhasz)
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bdsup2sub.h"
#include "ui_bdsup2sub.h"
#include "Subtitles/subtitleprocessor.h"
#include "types.h"
#include "conversiondialog.h"
#include "progressdialog.h"
#include "exportdialog.h"
#include "editdialog.h"
#include "helpdialog.h"
#include "colordialog.h"
#include "framepalettedialog.h"
#include "movedialog.h"
#include "Subtitles/subpicture.h"
#include "Tools/timeutil.h"
#include "Subtitles/palette.h"

#ifdef Q_OS_WIN
#include "windows.h"
#include "io.h"
#include "fcntl.h"
#endif

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMessageBox>
#else
#include <QMessageBox>
#endif

#include <QPixmap>
#include <QFileInfoList>
#include <QFileInfo>
#include <QSettings>
#include <QxtCore/QxtCommandOptions>

BDSup2Sub::BDSup2Sub(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDSup2Sub)
{
    ui->setupUi(this);
}

BDSup2Sub::~BDSup2Sub()
{
    delete ui;
}

void BDSup2Sub::closeEvent(QCloseEvent *event)
{
    settings->setValue("loadPath", QVariant(loadPath));
    settings->setValue("colorPath", QVariant(colorPath));

    subtitleProcessor->exit();
    QMainWindow::closeEvent(event);
}

void BDSup2Sub::keyPressEvent(QKeyEvent *event)
{
    if (subtitleProcessor->getNumberOfFrames() > 0)
    {
        if(QApplication::keyboardModifiers() == Qt::ControlModifier)
        {
            if (event->key() == Qt::Key_PageUp)
            {
                int currentIndex = ui->subtitleNumberComboBox->currentIndex();
                if (currentIndex > 0)
                {
                    ui->subtitleNumberComboBox->setCurrentIndex(--currentIndex);
                }
            }
            else if (event->key() == Qt::Key_PageDown)
            {
                int currentIndex = ui->subtitleNumberComboBox->currentIndex();
                if (currentIndex < (subtitleProcessor->getNumberOfFrames() - 1))
                {
                    ui->subtitleNumberComboBox->setCurrentIndex(++currentIndex);
                }
            }
        }
    }
    if(QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        if (event->key() == Qt::Key_C)
        {
            ui->consoleOutput->copy();
        }
    }
    QMainWindow::keyPressEvent(event);
}

void BDSup2Sub::changeWindowTitle(QString newTitle)
{
    this->setWindowTitle(newTitle);
}

void BDSup2Sub::onLoadingSubtitleFileFinished(const QString &errorString)
{
    if (!errorString.isEmpty())
    {
        errorDialog(errorString);
    }
    else
    {
        warningDialog();

        int num = subtitleProcessor->getNumberOfFrames();

        if (setImportedPalette)
        {
            subtitleProcessor->setCurrentSrcDVDPalette(importedPalette);
        }
        if (setLumaThreshold)
        {
            QVector<int> lumaThr = subtitleProcessor->getLuminanceThreshold();
            if (lumThr1 > 0)
            {
                lumaThr.replace(0, lumThr1);
            }
            if (lumThr2 > 0)
            {
                lumaThr.replace(1, lumThr2);
            }
            subtitleProcessor->setLuminanceThreshold(lumaThr);
        }
        if (setAlphaThreshold)
        {
            subtitleProcessor->setAlphaThreshold(alphaThreshold);
        }

        ui->subtitleNumberComboBox->blockSignals(true);
        ui->alphaThresholdComboBox->blockSignals(true);
        ui->medLowThresholdComboBox->blockSignals(true);
        ui->hiMedThresholdComboBox->blockSignals(true);

        ui->subtitleNumberComboBox->clear();

        subtitleNumberValidator = new QIntValidator(1, num, this);
        for (int i = 1; i <= num; ++i)
        {
            ui->subtitleNumberComboBox->addItem(QString::number(i));
        }
        ui->subtitleNumberComboBox->setValidator(subtitleNumberValidator);
        ui->subtitleNumberComboBox->setCurrentIndex(subIndex);
        ui->alphaThresholdComboBox->setCurrentIndex(subtitleProcessor->getAlphaThreshold());
        ui->hiMedThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[0]);
        ui->medLowThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[1]);

        ui->subtitleNumberComboBox->blockSignals(false);
        ui->alphaThresholdComboBox->blockSignals(false);
        ui->medLowThresholdComboBox->blockSignals(false);
        ui->hiMedThresholdComboBox->blockSignals(false);

        ui->subtitleLanguageComboBox->blockSignals(true);
        if (ui->subtitleLanguageComboBox->count() > 0)
        {
            ui->subtitleLanguageComboBox->setEnabled(true);
            ui->subtitleLanguageComboBox->setCurrentIndex(subtitleProcessor->getLanguageIdxRead());
        }
        else
        {
            ui->subtitleLanguageComboBox->setEnabled(false);
        }
        ui->subtitleLanguageComboBox->blockSignals(false);

        if (subtitleProcessor->getCropOfsY() > 0)
        {
            if (QMessageBox::question(this, "", "Reset Crop Offset?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            {
                subtitleProcessor->setCropOfsY(0);
            }
        }

        if (subtitleProcessor->getSubPictureSrc(0)->startTime() < 0)
        {
            QMessageBox::warning(this, "Warning!", QString("First subpicture has timestamp of %1.\n"
                                 "Please specify proper delay value to correct this.")
                                 .arg(TimeUtil::ptsToTimeStr(subtitleProcessor->getSubPictureSrc(0)->startTime())));
        }

        ConversionDialog conversionDialog(this, subtitleProcessor, settings);
        conversionDialog.enableOptionMove(subtitleProcessor->getMoveCaptions());
        if (conversionDialog.exec() != QDialog::Rejected && subtitleProcessor->getNumberOfFrames() > 0)
        {
            subtitleProcessor->scanSubtitles();
            if (subtitleProcessor->getMoveCaptions())
            {
                QThread *workerThread = new QThread;
                subtitleProcessor->moveToThread(workerThread);
                connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(moveAll()));
                connect(subtitleProcessor, SIGNAL(moveAllFinished(QString)), workerThread, SLOT(deleteLater()));

                subtitleProcessor->setActive(true);
                workerThread->start();
            }
            convertSup();
        }
        else
        {
            closeSubtitle();
            subtitleProcessor->close();
            print("Loading cancelled by user.");
        }
    }
}

void BDSup2Sub::onWritingSubtitleFileFinished(const QString& errorString)
{
    if (!errorString.isEmpty())
    {
        print(QString("ERROR: " + errorString));
        QMessageBox::warning(this, "Error!", errorString);
    }
    else
    {
        warningDialog();
    }
}

void BDSup2Sub::onMoveAllFinished(const QString &errorString)
{
    if (!errorString.isEmpty())
    {
        errorDialog(errorString);
    }
    else
    {
        try
        {
            subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
        }
        catch (QString e)
        {
            errorDialog(e);
            return;
        }

        refreshSrcFrame(subIndex);
        refreshTrgFrame(subIndex);
        ui->subtitleNumberComboBox->setCurrentIndex(subIndex);
    }
}

void BDSup2Sub::convertSup()
{
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshSrcFrame(subIndex);
    refreshTrgFrame(subIndex);

    enableCoreComponents(true);

    if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getInputMode() == InputMode::SUPIFO)
    {
        enableVobSubComponents(true);
    }
    else
    {
        enableVobSubComponents(false);
    }

    subtitleProcessor->loadedHook();
    subtitleProcessor->addRecent(loadPath);
    updateRecentMenu();
}

void BDSup2Sub::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void BDSup2Sub::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }
    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty() || !QFileInfo(fileName).isFile())
    {
        return;
    }
    closeFile();
    connectSubtitleProcessor();
    loadPath = fileName;
    loadSubtitleFile();
}

void BDSup2Sub::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (settings == 0)
    {
        loadSettings();
    }
    init();
    if (fromCLI && loadPath != "")
    {
        if (!QFile(loadPath).exists())
        {
            QMessageBox::warning(this, "File not found!", QString("File '%1' does not exist\n").arg(QDir::toNativeSeparators(loadPath)));
            return;
        }
        loadSubtitleFile();
    }
}

void BDSup2Sub::init()
{
    okBackground = new QPalette(ui->subtitleNumberComboBox->palette());
    errorBackground = new QPalette();
    errorBackground->setColor(QPalette::Base, QColor(0xffe1acac));

    this->setWindowTitle(progNameVer);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->consoleOutput->setFont(font);

    ui->consoleOutput->insertPlainText(progNameVer + " - a converter from Blu-Ray/HD-DVD SUP to DVD SUB/IDX and more\n");
    ui->consoleOutput->insertPlainText(authorDate + "\n");
    ui->consoleOutput->insertPlainText("Official thread at Doom9: https://forum.doom9.org/showthread.php?t=165416\n\n");

    if (subtitleProcessor == 0)
    {
        subtitleProcessor = new SubtitleProcessor(this, settings, true);
    }

    updateRecentMenu();

    fillComboBoxes();

    ui->action_Verbose_Output->setChecked(subtitleProcessor->getVerbatim());
    ui->action_Fix_invisible_frames->setChecked(subtitleProcessor->getFixZeroAlpha());
    ui->action_Swap_Cr_Cb->setChecked(subtitleProcessor->getSwapCrCb());

    connect(ui->action_Load, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->action_Save_Export, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->action_Close, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->action_Conversion_Settings, SIGNAL(triggered()), this, SLOT(openConversionSettings()));
    connect(ui->subtitleImage, SIGNAL(onMouseClicked(QMouseEvent*)), this, SLOT(onEditPaneClicked(QMouseEvent*)));
    connect(ui->action_Edit_Frame, SIGNAL(triggered()), this, SLOT(loadEditPane()));
    //connect(ui->action_Help, SIGNAL(triggered()), this, SLOT(loadHelpDialog()));
    connect(ui->action_Swap_Cr_Cb, SIGNAL(toggled(bool)), this, SLOT(swapCrCb_toggled(bool)));
    connect(ui->action_Verbose_Output, SIGNAL(toggled(bool)), this, SLOT(verbatimOutput_toggled(bool)));
    connect(ui->action_Fix_invisible_frames, SIGNAL(toggled(bool)), this, SLOT(fixInvisibleFrames_toggled(bool)));
    connect(ui->actionEdit_default_DVD_Palette, SIGNAL(triggered()), this, SLOT(editDefaultDVDPalette_triggered()));
    connect(ui->actionEdit_imported_DVD_Palette, SIGNAL(triggered()), this, SLOT(editImportedDVDPalette_triggered()));
    connect(ui->actionEdit_DVD_Frame_Palette, SIGNAL(triggered()), this, SLOT(editDVDFramePalette_triggered()));
    connect(ui->action_Move_all_captions, SIGNAL(triggered()), this, SLOT(moveAllCaptions_triggered()));
    connect(ui->action_Reset_crop_offset, SIGNAL(triggered()), this, SLOT(resetCropOffset_triggered()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(showAboutQt()));

    progressDialog = new ProgressDialog(this);
    connectSubtitleProcessor();
}

void BDSup2Sub::loadSettings()
{
    //Rename INI file to name matching the new program name
    QString oldIniFilePath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(oldIniName);
    QString newIniFilePath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(iniName);
    QFileInfo oldIniFileInfo = QFileInfo(oldIniFilePath);
    if (oldIniFileInfo.exists())
    {
        QDir dir = oldIniFileInfo.absoluteDir();
        dir.rename(oldIniFilePath, newIniFilePath);
        settings = new QSettings(newIniFilePath, QSettings::IniFormat);

        //Fix mangled Windows path names written by Java in INI files imported from original BDSup2Sub
#ifdef Q_OS_WIN
        QString fixPath;
        if (settings->allKeys().contains("loadPath"))
        {
            fixPath = settings->value("loadPath").toString();
            fixPath.insert(1, ":");
            settings->setValue("loadPath", QVariant(fixPath));
        }
        for (auto key : settings->allKeys())
        {
            if (key.contains("recent"))
            {
                fixPath = settings->value(key).toString();
                fixPath.insert(1, ":");
                settings->setValue(key, QVariant(fixPath));
            }
        }
        settings->sync();
#endif
    }
    else
    {
        settings = new QSettings(newIniFilePath, QSettings::IniFormat);
    }

    if (!fromCLI)
    {
        loadPath = settings->value("loadPath", "").toString();
        colorPath = settings->value("colorPath", "").toString();
    }
}

void BDSup2Sub::resizeEvent(QResizeEvent *event)
{
    ui->sourceImage->updateImage();
    ui->targetImage->updateImage();
    QMainWindow::resizeEvent(event);
}

void BDSup2Sub::connectSubtitleProcessor()
{
    if (subtitleProcessor != 0)
    {
        subtitleProcessor->moveToThread(QApplication::instance()->thread());
        subtitleProcessor->disconnect();
    }
    connect(subtitleProcessor, SIGNAL(windowTitleChanged(QString)), this, SLOT(changeWindowTitle(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogTextChanged(QString)), progressDialog, SLOT(setText(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogTitleChanged(QString)), progressDialog, SLOT(setWindowTitle(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogValueChanged(int)), progressDialog, SLOT(setCurrentValue(int)));
    connect(subtitleProcessor, SIGNAL(progressDialogVisibilityChanged(bool)), progressDialog, SLOT(setVisible(bool)));
    connect(progressDialog, SIGNAL(operationCancelled()), this, SLOT(onOperationCancelled()));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished(QString)), this, SLOT(onLoadingSubtitleFileFinished(QString)));
    connect(subtitleProcessor, SIGNAL(writingSubtitleFinished(QString)), this, SLOT(onWritingSubtitleFileFinished(QString)));
    connect(subtitleProcessor, SIGNAL(moveAllFinished(QString)), this, SLOT(onMoveAllFinished(QString)));
    connect(subtitleProcessor, SIGNAL(printText(QString)), this, SLOT(print(QString)));
}

void BDSup2Sub::fillComboBoxes()
{
    ui->alphaThresholdComboBox->blockSignals(true);
    ui->medLowThresholdComboBox->blockSignals(true);
    ui->hiMedThresholdComboBox->blockSignals(true);
    ui->outputFormatComboBox->blockSignals(true);
    ui->filterComboBox->blockSignals(true);
    ui->paletteComboBox->blockSignals(true);
    for (int i = 0; i < 256; ++i)
    {
        ui->alphaThresholdComboBox->addItem(QString::number(i));
        ui->medLowThresholdComboBox->addItem(QString::number(i));
        ui->hiMedThresholdComboBox->addItem(QString::number(i));
    }
    ui->alphaThresholdComboBox->setCurrentIndex(subtitleProcessor->getAlphaThreshold());
    ui->medLowThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[0]);
    ui->hiMedThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[1]);

    ui->outputFormatComboBox->setCurrentIndex((int)subtitleProcessor->getOutputMode());
    ui->filterComboBox->setCurrentIndex((int)subtitleProcessor->getScalingFilter());
    ui->paletteComboBox->setCurrentIndex((int)subtitleProcessor->getPaletteMode());

    ui->alphaThresholdComboBox->setValidator(alphaThresholdValidator);
    ui->medLowThresholdComboBox->setValidator(medLowThresholdValidator);
    ui->hiMedThresholdComboBox->setValidator(hiMedThresholdValidator);

    ui->alphaThresholdComboBox->blockSignals(false);
    ui->medLowThresholdComboBox->blockSignals(false);
    ui->hiMedThresholdComboBox->blockSignals(false);
    ui->outputFormatComboBox->blockSignals(false);
    ui->filterComboBox->blockSignals(false);
    ui->paletteComboBox->blockSignals(false);
}

void BDSup2Sub::enableCoreComponents(bool enable)
{
    ui->action_Load->setEnabled(enable);
    ui->menu_Recent_Files->setEnabled(enable && subtitleProcessor->getRecentFiles().size() > 0);
    ui->action_Save_Export->setEnabled(enable && subtitleProcessor->getNumberOfFrames() > 0);
    ui->action_Close->setEnabled(enable);
    ui->action_Edit_Frame->setEnabled(enable);
    ui->action_Move_all_captions->setEnabled(enable);
    ui->subtitleNumberComboBox->setEnabled(enable);
    ui->outputFormatComboBox->setEnabled(enable);
    ui->filterComboBox->setEnabled(enable);
}

void BDSup2Sub::enableVobSubComponents(bool enable)
{
    ui->paletteComboBox->blockSignals(true);
    ui->paletteComboBox->clear();
    ui->paletteComboBox->addItem("keep existing");
    ui->paletteComboBox->addItem("create new");
    if (!enable)
    {
        ui->paletteComboBox->addItem("dithered");
    }
    if (!enable && subtitleProcessor->getPaletteMode() != PaletteMode::CREATE_DITHERED)
    {
        ui->paletteComboBox->setCurrentIndex((int)subtitleProcessor->getPaletteMode());
    }
    else
    {
        ui->paletteComboBox->setCurrentIndex((int)PaletteMode::KEEP_EXISTING);
    }

    if (!enable || subtitleProcessor->getInputMode() == InputMode::VOBSUB || subtitleProcessor->getInputMode() == InputMode::SUPIFO)
    {
        ui->paletteComboBox->setEnabled(true);
    }
    else
    {
        ui->paletteComboBox->setEnabled(false);
    }

    enableVobSubMenuCombo();
    ui->paletteComboBox->blockSignals(false);
}

void BDSup2Sub::enableVobSubMenuCombo()
{
    bool enable;

    if ((subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getOutputMode() == OutputMode::SUPIFO) &&
       ((subtitleProcessor->getInputMode() != InputMode::VOBSUB || subtitleProcessor->getInputMode() != InputMode::SUPIFO) ||
         subtitleProcessor->getPaletteMode() != PaletteMode::KEEP_EXISTING))
    {
        enable = true;
    }
    else
    {
        enable = false;
    }

    ui->alphaThresholdComboBox->setEnabled(enable);
    ui->hiMedThresholdComboBox->setEnabled(enable);
    ui->medLowThresholdComboBox->setEnabled(enable);

    if (subtitleProcessor->getInputMode() == InputMode::VOBSUB || subtitleProcessor->getInputMode() == InputMode::SUPIFO)
    {
        enable = true;
    }
    else
    {
        enable = false;
    }

    ui->actionEdit_imported_DVD_Palette->setEnabled(enable);
    ui->actionEdit_DVD_Frame_Palette->setEnabled(enable);
}

void BDSup2Sub::openFile()
{
    if (!loadPath.isEmpty())
    {
        closeFile();
        connectSubtitleProcessor();
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"),
                                                    loadPath.isEmpty() ? QApplication::applicationDirPath() : QFileInfo(loadPath).absolutePath(),
                                                    filter,
                                                    &selectedFilter
                                                    );

    if (fileName.isNull() || fileName.isEmpty() || !QFile(fileName).exists()) return;

    loadPath = fileName;

    loadSubtitleFile();
}

void BDSup2Sub::saveFile()
{
    QString path = savePath + QDir::separator() + saveFileName + "_exp.";
    if (ui->outputFormatComboBox->currentText().contains("IDX"))
    {
        path += "idx";
    }
    else if (ui->outputFormatComboBox->currentText().contains("IFO"))
    {
        path += "ifo";
    }
    if (ui->outputFormatComboBox->currentText().contains("BD"))
    {
        path += "sup";
    }
    if (ui->outputFormatComboBox->currentText().contains("XML"))
    {
        path += "xml";
    }
    if (setLangIdx)
    {
        subtitleProcessor->setLanguageIdx(langIdx);
    }
    ExportDialog exportDialog(this, path, subtitleProcessor);
    if (exportDialog.exec() != QDialog::Rejected)
    {
        QString fileName = exportDialog.getFileName();
        QFileInfo fileInfo(fileName);
        savePath = fileInfo.absolutePath();
        saveFileName = fileInfo.completeBaseName();
        saveFileName = saveFileName.replace(QRegExp("_exp$"), "");
        QString fi, fs;

        if (ui->outputFormatComboBox->currentText().contains("IDX"))
        {
            fi = savePath + QDir::separator() + fileInfo.completeBaseName() + ".idx";
            fs = savePath + QDir::separator() + fileInfo.completeBaseName() + ".sub";
        }
        else if (ui->outputFormatComboBox->currentText().contains("IFO"))
        {
            fi = savePath + QDir::separator() + fileInfo.completeBaseName() + ".ifo";
            fs = savePath + QDir::separator() + fileInfo.completeBaseName() + ".sup";
        }
        if (ui->outputFormatComboBox->currentText().contains("BD"))
        {
            fs = savePath + QDir::separator() + fileInfo.completeBaseName() + ".sup";
            fi = fs;
        }
        if (ui->outputFormatComboBox->currentText().contains("XML"))
        {
            fs = savePath + QDir::separator() + fileInfo.completeBaseName() + ".xml";
            fi = fs;
        }
        if (QFile(fi).exists() || QFile(fs).exists())
        {
            if ((QFile(fi).exists() && !QFileInfo(fi).isWritable()) ||
                (QFile(fs).exists() && !QFileInfo(fs).isWritable()))
            {
                QMessageBox::warning(this, "", "Target is write protected.");
                return;
            }
            if (QMessageBox::question(this, "", "Target exists! Overwrite?",
                                      QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            {
                QMessageBox::warning(this, "", "Target exists. Aborted by user.");
                return;
            }
        }
        connectSubtitleProcessor();
        QThread *workerThread = new QThread;
        subtitleProcessor->setLoadPath(fileName);
        subtitleProcessor->moveToThread(workerThread);

        connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(createSubtitleStream()));
        connect(subtitleProcessor, SIGNAL(writingSubtitleFinished(QString)), workerThread, SLOT(deleteLater()));

        subtitleProcessor->setActive(true);
        workerThread->start();
    }
}

void BDSup2Sub::closeFile()
{
    closeSubtitle();
    subtitleProcessor->close();
    ui->sourceImage->setImage(QImage(0,0));
    ui->sourceImage->update();
    ui->targetImage->setImage(QImage(0,0));
    ui->targetImage->update();
}

void BDSup2Sub::onOperationCancelled()
{
    subtitleProcessor->setActive(false);
}

void BDSup2Sub::onAddLanguage(const QString &language)
{
    ui->subtitleLanguageComboBox->blockSignals(true);
    ui->subtitleLanguageComboBox->addItem(language);
    ui->subtitleLanguageComboBox->blockSignals(false);
}

void BDSup2Sub::loadSubtitleFile()
{
    QFileInfo fileInfo(loadPath);
    QString extension = fileInfo.suffix().toLower();

    bool isXML(extension == "xml"), isIDX(extension == "idx"), isIFO(extension == "ifo");

    QByteArray id = subtitleProcessor->getFileID(loadPath, 4);
    StreamID streamID = (id.isEmpty()) ? StreamID::UNKNOWN : subtitleProcessor->getStreamID(id);

    if (extension == "sup" && streamID == StreamID::SUP)
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open IFO file"),
                                                        loadPath.isEmpty() ? QApplication::applicationDirPath() : QFileInfo(loadPath).absolutePath(),
                                                        ifoFilter,
                                                        &ifoFilter
                                                        );

        if (!fileName.isNull() && !fileName.isEmpty() && QFileInfo(fileName).exists())
        {
            if (QFileInfo(fileName).suffix().toLower() == "ifo")
            {
                subtitleProcessor->setIFOFileName(fileName);
            }
            else
            {
                QMessageBox::warning(this, "Error!", "Invalid IFO file specified. Ignoring.");
            }
        }
    }

    if (!isXML && !isIDX && !isIFO && streamID == StreamID::UNKNOWN)
    {
        QMessageBox::warning(this, "Wrong format!", "This is not a supported SUP stream");
        return;
    }

    ui->subtitleLanguageComboBox->blockSignals(true);
    ui->subtitleLanguageComboBox->clear();
    ui->subtitleLanguageComboBox->setEnabled(false);
    ui->subtitleLanguageComboBox->blockSignals(false);

    subIndex = 0;
    saveFileName = QFileInfo(loadPath).completeBaseName();
    savePath = QFileInfo(loadPath).absolutePath();

    enableCoreComponents(false);
    enableVobSubComponents(false);

    subtitleProcessor->setLoadPath(loadPath);

    connect(subtitleProcessor, SIGNAL(addLanguage(QString)), this, SLOT(onAddLanguage(QString)));

    QThread *workerThread = new QThread;
    subtitleProcessor->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(readSubtitleStream()));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished(QString)), workerThread, SLOT(deleteLater()));

    subtitleProcessor->setActive(true);
    workerThread->start();
}

QString BDSup2Sub::getWarningMessage()
{
    int warnings = subtitleProcessor->getWarnings();
    subtitleProcessor->resetWarnings();
    int errors = subtitleProcessor->getErrors();
    subtitleProcessor->resetErrors();

    QString message;

    if (warnings + errors > 0)
    {
        if (warnings > 0)
        {
            if (warnings == 1)
            {
                message += QString(QString::number(warnings) + " warning");
            }
            else
            {
                message += QString(QString::number(warnings) + " warnings");
            }
        }
        if (warnings > 0 && errors > 0)
        {
            message += " and ";
        }
        if (errors > 0)
        {
            if (errors == 1)
            {
                message += QString(QString::number(errors) + " error");
            }
            else
            {
                message += QString(QString::number(errors) + " errors");
            }
        }

        if (warnings + errors < 3)
        {
            message = "There was " + message;
        }
        else
        {
            message = "There were " + message;
        }
    }
    return message;
}

void BDSup2Sub::warningDialog()
{
    QString message = getWarningMessage();
    if (!message.isNull() || !message.isEmpty())
    {
        QMessageBox::warning(this, "Warnings!", QString(message + "\nCheck the log for details"));
    }
}

void BDSup2Sub::errorDialog(const QString &errorMessage)
{
    print(QString("ERROR: %1\n").arg(errorMessage));
    QMessageBox::warning(this, "Error!", errorMessage);
}

void BDSup2Sub::printWarnings(QTextStream &stream)
{
    QString message = getWarningMessage();
    if (!message.isNull() || !message.isEmpty())
    {
        stream << QString(message + "\n");
    }
}

void BDSup2Sub::showUsage(QTextStream& outStream)
{
    outStream << progNameVer << " " << authorDate << endl;
    outStream << "Syntax:" << endl;
    outStream << QString("%1 [options] -o outfile infile").arg(progName.toLower()) << endl;
    options->showUsage(false, outStream);
    outStream << endl << "Wildcard support:" << endl;
    outStream << "Use \"*\" for any character and \"?\" for one character in the source name" << endl;
    outStream << "Use exactly one \"*\" in the target file name." << endl;
    outStream << "Example:" << endl;
    outStream << "bdsup2sub++ --resolution 720 --fps-target 25p -o dvd_*.sub 'movie* 1?.sup'" << endl;
}

void BDSup2Sub::addCLIOptions()
{
    options = new QxtCommandOptions;
    options->setParamStyle(QxtCommandOptions::Space);
    options->setFlagStyle(QxtCommandOptions::DoubleDash);

    options->addSection("Options");
    options->add("h",                 "\tList options");
    options->alias("h", "help");
    options->add("load-settings",     "\tSet to load settings stored in INI file.");
    options->add("resolution",        "\tSet resolution to 480, 576, 720 or 1080. Default: 576. "
                                      "\tSupported values: keep, ntsc=480, pal=576, 1440x1080.",
                 QxtCommandOptions::ValueRequired);
    options->add("fps-source",        "\tSynchronize source frame rate to <x>. Default: auto. "
                                      "\tSupported values: 24p=23.976, 25p=25, 30p=29.967.",
                 QxtCommandOptions::ValueRequired);
    options->add("fps-target",        "\tConvert the target frame rate to <x>. Default: keep. "
                                      "\tSupported values: 24p=23.976, 25p=25, 30p=29.967.",
                 QxtCommandOptions::ValueRequired);
    options->add("delay",             "\tSet delay in ms. Default: 0.0. " ,
                 QxtCommandOptions::ValueRequired);
    options->add("filter",            "\tSet the filter to use for scaling. Default: bilinear. "
                                      "\tSupported values: bilinear, triangle, bicubic, bell, "
                                      "\tb-spline, hermite, lanczos3, mitchell.",
                 QxtCommandOptions::ValueRequired);
    options->add("palette-mode",      "\tPalette mode: keep, create, dither. Default: create. " ,
                 QxtCommandOptions::ValueRequired);
    options->add("minimum-time",      "\tSet the minimum display time in ms. Default: 500.",
                 QxtCommandOptions::ValueRequired);
    options->add("merge-time",        "\tSet max time diff to merge subs in ms. Default: 200.",
                 QxtCommandOptions::ValueRequired);
    options->add("move-in-ratio",     "\tMove captions from inside screen ratio <x>.",
                 QxtCommandOptions::ValueRequired, 1);
    options->add("move-out-ratio",    "\tMove captions from outside screen ratio <x>.",
                 QxtCommandOptions::ValueRequired, 1);
    options->add("move-y-offset",     "\tSet optional +/- offset to move captions by.",
                 QxtCommandOptions::ValueRequired);
    options->add("move-x",            "\tMove captions horizontally from specified position. "
                                      "\tSupported values: left, right, center, origin.",
                 QxtCommandOptions::ValueRequired);
    options->add("move-x-offset",     "\tSet optional +/- offset to move captions by.",
                 QxtCommandOptions::ValueRequired);
    options->add("crop-y",            "\tCrop the upper/lower n lines. Default: 0",
                 QxtCommandOptions::ValueRequired);
    options->add("alpha-crop",        "\tSet the alpha cropping threshold. Default: 10",
                 QxtCommandOptions::ValueRequired);
    options->add("scale-x",           "\tScale captions horizontally by factor. Default 1.0.",
                 QxtCommandOptions::ValueRequired);
    options->add("scale-y",           "\tScale captions vertically by factor. Default 1.0.",
                 QxtCommandOptions::ValueRequired);
    options->add("no-export-palette", "\tDo not export palette file.");
    options->add("export-palette",    "\tExport target palette in PGCEdit format.");
    options->add("forced-only",       "\tExport only forced subtitles.");
    options->add("force-all",         "\tSet or clear the forced flag for all subpictures. "
                                      "\tSupported values: set/clear.",
                 QxtCommandOptions::ValueRequired);
    options->add("swap",              "\tSwap Cr/Cb components.");
    options->add("no-fix-invisible",  "\tDo not fix zero alpha frame palette.");
    options->add("fix-invisible",     "\tFix zero alpha frame palette.");
    options->add("no-verbatim",       "\tSwitch off verbose console output mode.");
    options->add("verbatim",          "\tSwitch on verbose console output mode.");
    options->add("log-to-stderr",     "\tSwitch to change progress output to standard error.");

    options->addSection("Options only for SUB/IDX or SUP/IFO as target");
    options->add("alpha-thr",         "\tSet alpha threshold 0..255. Default 80.",
                 QxtCommandOptions::ValueRequired);
    options->add("med-low-thr",       "\tSet luminance low/med threshold 0..255.",
                 QxtCommandOptions::ValueRequired);
    options->add("med-hi-thr",        "\tSet luminance med/hi threshold 0..255.",
                 QxtCommandOptions::ValueRequired);
    options->add("language",          "\tSet language to <n>. Default: de (Vobsub Only).",
                 QxtCommandOptions::ValueRequired);
    options->add("palette-file",      "\tLoad palette file <n>. Overrides default palette.",
                 QxtCommandOptions::ValueRequired);

    options->addSection("Output");
    options->add("o",                 "\tSpecify output file.",
                 QxtCommandOptions::ValueRequired);
    options->alias("o", "output");
}

bool BDSup2Sub::execCLI(int argc, char** argv)
{
    Redirect_console();

    QTextStream outStream(stdout);
    QTextStream errorStream(stderr);

    addCLIOptions();

    QStringList args = QApplication::arguments();

    if (args.contains("-o"))
    {
        args.replace(args.indexOf("-o"), "--output");
    }

    options->parse(args);

    QMultiHash<QString, QVariant> parameters = options->parameters();
    QStringList positional = options->positional();

    if (positional.size() > 1 || options->count("h") || options->showUnrecognizedWarning(errorStream))
    {
        showUsage(outStream);
        exit(1);
    }
    else if (parameters.size() == 0)
    {
        if (QFileInfo(positional[0]).exists())
        {
            fromCLI = true;
            loadPath = positional[0];
            return false;
        }
        else
        {
            errorStream << QString("ERROR: File '%1' does not exist.").arg(positional[0]) << endl;
            exit(1);
        }
    }
    else
    {
        if (options->count("log-to-stderr"))
        {
            QFile file;
            file.open(stderr, QIODevice::WriteOnly);
            outStream.setDevice(&file);
        }
        outStream << progNameVer + "\n";
        // parse parameters
        QString src = positional.size() == 1 ? positional[0] : "";
        QString trg = "";
        OutputMode mode = (OutputMode)0;

        if (options->count("o"))
        {
            trg = options->value("o").toString();
            QString ext = QFileInfo(trg).suffix();
            if (ext.isEmpty())
            {
                errorStream << QString("ERROR: No extension given for target %1").arg(trg) << endl;
                exit(1);
            }
            if (ext == "sup")
            {
                mode = OutputMode::BDSUP;
            }
            else if (ext == "sub" || ext == "idx")
            {
                mode = OutputMode::VOBSUB;
            }
            else if (ext == "xml")
            {
                mode = OutputMode::XML;
            }
            else if (ext == "ifo")
            {
                mode = OutputMode::SUPIFO;
            }
            else
            {
                errorStream << QString("ERROR: Unknown extension of target %1").arg(trg) << endl;
                exit(1);
            }
        }

        // handle wildcards
        QStringList srcFileNames;
        QStringList trgFileNames;
        QString path;
        // multiple files
        if (src.indexOf('*') != -1)
        {
            path = QFileInfo(src).path();
            if (path == ".")
            {
                path = path + QDir::separator();
            }
            QFileInfo srcInfo(src);
            QDir dir(srcInfo.absolutePath());
            QStringList filters(srcInfo.completeBaseName());
            QFileInfoList srcFiles = dir.entryInfoList(filters, QDir::Files);

            if (srcFiles.size() == 0)
            {
                errorStream << QString("ERROR: No match found for '%1'").arg(src) << endl;
                exit(1);
            }
            if (trg.indexOf('*') == -1)
            {
                errorStream << "ERROR: No wildcards in target string!" << endl;
                exit(1);
            }
            for (int i = 0; i < srcFiles.size(); ++i)
            {
                srcFileNames.insert(i, srcFiles.at(i).absoluteFilePath());
                QString srcName = srcFiles.at(i).completeBaseName();
                trgFileNames.insert(i, QFileInfo(QString(trg).replace("*", srcName)).absoluteFilePath());
            }
        }
        else
        {
            if (src != "")
            {
                srcFileNames.insert(0, QFileInfo(src).absoluteFilePath());
            }
            int aPos = trg.indexOf('*');
            if (aPos != -1)
            {
                // replace asterisk by path + filename of source without the extension
                trgFileNames.insert(0, QFileInfo(trg.replace("*", QFileInfo(src).completeBaseName())).absoluteFilePath());
            }
            else
            {
                if (trg != "")
                {
                    trgFileNames.insert(0, QFileInfo(trg).absoluteFilePath());
                }
            }
        }

        //Load GUI settings
        loadSettings();
        if (options->count("load-settings"))
        {
            subtitleProcessor = new SubtitleProcessor(0, settings, true);
        }
        else
        {
            subtitleProcessor = new SubtitleProcessor(0, settings, false);
        }

        subtitleProcessor->setOutputMode(mode);

        QString value;
        bool ok;
        int ival;

        if (options->count("alpha-thr"))
        {
            value = options->value("alpha-thr").toString();
            ival = value.toInt(&ok);
            ival = ok ? ival : -1;
            if (ival < 0 || ival > 255)
            {
                errorStream << QString("ERROR: Illegal number range for alpha threshold: %1").arg(value) << endl;
                exit(1);
            }
            else
            {
                alphaThreshold = ival;
                setAlphaThreshold = true;
            }
            outStream << QString("OPTION: Set alpha threshold to %1").arg(value) << endl;
        }

        if (options->count("med-low-thr"))
        {
            value = options->value("med-low-thr").toString();
            ival = value.toInt(&ok);
            ival = ok ? ival : -1;
            if (ival <0 || ival > 255)
            {
                errorStream << QString("ERROR: Illegal number range for luminance: %1").arg(value) << endl;
                exit(1);
            }
            else
            {
                lumThr1 = ival;
                setLumaThreshold = true;
            }
            outStream << QString("OPTION: Set med/low luminance threshold to %1").arg(value) << endl;
        }

        if (options->count("med-hi-thr"))
        {
            value = options->value("med-hi-thr").toString();
            ival = value.toInt(&ok);
            ival = ok ? ival : -1;
            if (ival <0 || ival > 255)
            {
                errorStream << QString("ERROR: Illegal number range for luminance: %1").arg(value) << endl;
                exit(1);
            }
            else
            {
                lumThr2 = ival;
                setLumaThreshold = true;
            }
            outStream << QString("OPTION: Set med/hi luminance threshold to %1").arg(value) << endl;
        }

        if (options->count("resolution"))
        {
            value = options->value("resolution").toString().toLower();

            bool isKeep = value == "keep";
            subtitleProcessor->setConvertResolution(!isKeep);

            if (!isKeep)
            {
                Resolution resolution;
                bool defineFPStrg = options->count("fps-target");
                ival = value.toInt(&ok);
                ival = ok ? ival : -1;
                if (value == "pal" || ival == 576)
                {
                    resolution = Resolution::PAL;
                    if (!defineFPStrg)
                    {
                        subtitleProcessor->setFPSTrg(FPS_PAL);
                    }
                }
                else if (value == "ntsc" || ival == 480)
                {
                    resolution = Resolution::NTSC;
                    if (!defineFPStrg)
                    {
                        subtitleProcessor->setFPSTrg(FPS_NTSC);
                    }
                }
                else if (value == "720p" || ival == 720)
                {
                    resolution = Resolution::HD_720;
                    if (!defineFPStrg)
                    {
                        subtitleProcessor->setFPSTrg(FPS_24P);
                    }
                }
                else if (value == "1440x1080")
                {
                    resolution = Resolution::HD_1440x1080;
                    if (!defineFPStrg)
                    {
                        subtitleProcessor->setFPSTrg(FPS_24P);
                    }
                }
                else if (value == "1080p" || ival == 1080)
                {
                    resolution = Resolution::HD_1080;
                    if (!defineFPStrg)
                    {
                        subtitleProcessor->setFPSTrg(FPS_24P);
                    }
                }
                else
                {
                    errorStream << QString("ERROR: Illegal resolution: %1")
                                   .arg(value) << endl;
                    exit(1);
                }
                subtitleProcessor->setOutputResolution(resolution);
                outStream << QString("OPTION: Set resolution to %1")
                             .arg(subtitleProcessor->getResolutionName(resolution)) << endl;
            }
        }

        if (options->count("language"))
        {
            value = options->value("language").toString();
            for (int l = 0; l < subtitleProcessor->getLanguages().size(); ++l)
            {
                if (subtitleProcessor->getLanguages()[l][1] == value)
                {
                    langIdx = l;
                    setLangIdx = true;
                    break;
                }
            }
            if (langIdx == -1)
            {
                errorStream << QString("ERROR: Unknown language %1").arg(value) << endl;
                errorStream << "Use one of the following 2 character codes:" << endl;
                for (int l = 0; l < subtitleProcessor->getLanguages().size(); ++l)
                {
                    errorStream << QString("    %1 - %2")
                                   .arg(subtitleProcessor->getLanguages()[l][1])
                                   .arg(subtitleProcessor->getLanguages()[l][0]) << endl;
                }
                exit(1);
            }
            outStream << QString("OPTION: Set language to %1 (%2)")
                         .arg(subtitleProcessor->getLanguages()[langIdx][0])
                         .arg(subtitleProcessor->getLanguages()[langIdx][1]) << endl;
            subtitleProcessor->setLanguageIdxSet(true);
        }

        if (options->count("palette-file"))
        {
            value = options->value("palette-file").toString();
            QFileInfo f(value);
            if (!f.exists())
            {
                errorStream << QString("ERROR: No palette file found at: %1").arg(value) << endl;
                exit(1);
            }
            QSettings colorSettings(value, QSettings::IniFormat);
            if (!colorSettings.allKeys().contains("Color_0"))
            {
                errorStream << "ERROR: Not a valid palette file" << endl;
                exit(1);
            }

            setImportedPalette = true;
            importedPalette = new Palette(subtitleProcessor->getDefaultDVDPalette());

            for (int c = 0; c < 15; ++c)
            {
                QVariantList defaultList = { 0, 0, 0 };
                QVariantList s = colorSettings.value(QString("Color_%1").arg(QString::number(c)),
                                                 defaultList).toList();
                if (s.size() >= 3)
                {
                    int red = s[0].toInt();
                    int green = s[1].toInt();
                    int blue = s[2].toInt();
                    importedPalette.setColor(c + 1, QColor(red, green, blue, 0));
                }
            }
            outStream << QString("OPTION: Loaded palette from %1").arg(value) << endl;
        }

        if (options->count("forced-only"))
        {
            subtitleProcessor->setExportForced(true);
            outStream << "OPTION: Exporting only forced subtitles." << endl;
        }

        if (options->count("swap"))
        {
            subtitleProcessor->setSwapCrCb(true);
            outStream << "OPTION: Swapping Cr/Cb components." << endl;
        }

        if (options->count("fps-source"))
        {
            value = options->value("fps-source").toString().toLower();
            if (value != "auto")
            {
                double fps = subtitleProcessor->getFPS(value);

                if (fps > 0)
                {
                    subtitleProcessor->setFPSSrc(fps, true);
                }
                else
                {
                    errorStream << QString("ERROR: Invalid source framerate: %1")
                                   .arg(value) << endl;
                    exit(1);
                }
            }
            outStream << QString("OPTION: synchronize target framerate to %1").arg(value) << endl;
        }

        if (options->count("fps-target"))
        {
            value = options->value("fps-target").toString().toLower();
            if (value != "keep")
            {
                double fps = subtitleProcessor->getFPS(value);

                if (fps > 0)
                {
                    subtitleProcessor->setKeepFps(false);
                    subtitleProcessor->setConvertFPS(true);
                    subtitleProcessor->setFPSTrg(fps);
                }
                else
                {
                    errorStream << QString("ERROR: Invalid target framerate: %1")
                                   .arg(value) << endl;
                    exit(1);
                }
                outStream << QString("OPTION: Converting framerate from %1fps to %2fps")
                             .arg(QString::number(subtitleProcessor->getFPSSrc(), 'g', 6))
                             .arg(QString::number(subtitleProcessor->getFPSTrg(), 'g', 6)) << endl;
            }
        }

        if (options->count("delay"))
        {
            double delay;
            bool ok;

            value = options->value("delay").toString();
            delay = value.toDouble(&ok) * 90.0;

            if (!ok)
            {
                errorStream << QString("ERROR: Illegal delay value: %1").arg(value) << endl;
                exit(1);
            }
            int delayPTS = (int)subtitleProcessor->syncTimePTS((long)delay, subtitleProcessor->getFPSTrg());
            subtitleProcessor->setDelayPTS(delayPTS);
            outStream << QString("OPTION: Set delay to %1")
                         .arg(QString::number(delayPTS / 90.0, 'g', 6)) << endl;
        }

        if (options->count("minimum-time"))
        {
            double time;
            bool ok;

            value = options->value("minimum-time").toString();
            time = value.toDouble(&ok) * 90.0;

            if (!ok)
            {
                errorStream << QString("ERROR: Illegal value for minimum display time: %1")
                               .arg(value) << endl;
                exit(1);
            }
            int tMin = (int)subtitleProcessor->syncTimePTS((long)time, subtitleProcessor->getFPSTrg());
            subtitleProcessor->setMinTimePTS(tMin);
            subtitleProcessor->setFixShortFrames(true);
            outStream << QString("OPTION: Set minimum display time to %1")
                         .arg(QString::number(tMin / 90.0, 'g', 6)) << endl;
        }

        double screenRatio = -1;
        if (options->count("move-in-ratio") || options->count("move-out-ratio"))
        {
            QString sm;
            if (options->count("move-in-ratio"))
            {
                value = options->value("move-in-ratio").toString();
                subtitleProcessor->setMoveModeY(MoveModeY::INSIDE);
                sm = "inside";
            }
            else if (options->count("move-out-ratio"))
            {
                value = options->value("move-out-ratio").toString();
                subtitleProcessor->setMoveModeY(MoveModeY::OUTSIDE);
                sm = "outside";
            }
            bool ok;
            screenRatio = value.toDouble(&ok);
            screenRatio = ok ? screenRatio : -1;

            if (screenRatio <= (16.0 / 9))
            {
                errorStream << QString("ERROR: Invalid screen ratio: %1").arg(value) << endl;
                exit(1);
            }

            if (options->count("move-y-offset"))
            {
                bool ok;
                value = options->value("move-y-offset").toString();
                int moveOffsetY = value.toInt(&ok);
                if (!ok)
                {
                    errorStream << QString("ERROR: Invalid pixel offset: %1").arg(value) << endl;
                    exit(1);
                }
                subtitleProcessor->setMoveOffsetY(moveOffsetY);
            }

            outStream << QString("OPTION: Moving captions %1 %2:1 plus/minus %3 pixels")
                         .arg(sm)
                         .arg(QString::number(screenRatio, 'g', 6))
                         .arg(QString::number(subtitleProcessor->getMoveOffsetY())) << endl;
        }

        if (options->count("move-x"))
        {
            value = options->value("move-x").toString().toLower();

            if (value == "left")
            {
                subtitleProcessor->setMoveModeX(MoveModeX::LEFT);
            }
            else if (value == "right")
            {
                subtitleProcessor->setMoveModeX(MoveModeX::RIGHT);
            }
            else if (value == "center")
            {
                subtitleProcessor->setMoveModeX(MoveModeX::CENTER);
            }
            else if (value == "origin")
            {
                subtitleProcessor->setMoveModeX(MoveModeX::ORIGIN);
            }
            else
            {
                errorStream << QString("ERROR: Invalid moveX command: %1").arg(value) << endl;
                exit(1);
            }

            QString mx = value;

            if (options->count("move-x-offset"))
            {
                bool ok;
                value = options->value("move-x-offset").toString();
                int moveOffsetX = value.toInt(&ok);
                if (!ok)
                {
                    errorStream << QString("ERROR: Invalid pixel offset: %1").arg(value) << endl;
                    exit(1);
                }
                subtitleProcessor->setMoveOffsetX(moveOffsetX);
            }
            outStream << QString("OPTION: Moving captions to the %1 plus/minus %2 pixels")
                         .arg(mx)
                         .arg(QString::number(subtitleProcessor->getMoveOffsetX())) << endl;
        }

        if (options->count("crop-y"))
        {
            bool ok;
            int cropY;
            value = options->value("crop-y").toString();
            cropY = value.toInt(&ok);

            if (ok && cropY > 0)
            {
                subtitleProcessor->setCropOfsY(cropY);
                outStream << QString("OPTION: Set delay to %1").arg(value) << endl;
            }
            else
            {
                errorStream << QString("ERROR: Invalid crop y value: %1").arg(value) << endl;
            }
        }

        if (options->count("palette-mode"))
        {
            value = options->value("palette-mode").toString().toLower();

            if (value == "keep")
            {
                subtitleProcessor->setPaletteMode(PaletteMode::KEEP_EXISTING);
            }
            else if (value.toLower() == "create")
            {
                subtitleProcessor->setPaletteMode(PaletteMode::NEW);
            }
            else if (value.toLower() == "dither")
            {
                subtitleProcessor->setPaletteMode(PaletteMode::CREATE_DITHERED);
            }
            else
            {
                errorStream << QString("ERROR: Invalid palette mode: %1").arg(value) << endl;
                exit(1);
            }
            outStream << QString("OPTION: Set palette mode to %1").arg(value) << endl;
        }

        if (options->count("verbatim"))
        {
            subtitleProcessor->setVerbatim(true);
            outStream << QString("OPTION: Enabled verbose output.") << endl;
        }
        if (options->count("no-verbatim"))
        {
            subtitleProcessor->setVerbatim(false);
            outStream << QString("OPTION: Disabled verbose output.") << endl;
        }

        if (options->count("filter"))
        {
            value = options->value("filter").toString().toLower();

            value[0] = value[0].toUpper();
            int idx = scalingFilters.indexOf(value);
            if (idx != -1)
            {
                subtitleProcessor->setScalingFilter((ScalingFilters)idx);
                outStream << QString("OPTION: Set scaling filter to: %1").arg(value) << endl;
            }
            else
            {
                errorStream << QString("ERROR: Invalid scaling filter: %1").arg(value) << endl;
                exit(1);
            }
        }

        if (options->count("merge-time"))
        {
            bool ok;
            double time = 0;
            value = options->value("merge-time").toString();
            time = value.trimmed().toDouble(&ok) * 90.0;

            if (!ok)
            {
                errorStream << QString("ERROR: Illegal value for maximum merge time: %1").arg(value) << endl;
                exit(1);
            }
            int ti = (int)(time + 0.5);
            subtitleProcessor->setMergePTSdiff(ti);
            outStream << QString("OPTION: Set maximum merge time to %1").arg(QString::number(ti / 90.0, 'g', 6)) << endl;
        }

        if (options->count("scale-x") || options->count("scale-y"))
        {
            bool ok;
            double scaleX = 1.0;
            if (options->count("scale-x"))
            {
                value = options->value("scale-x").toString();
                scaleX = value.toDouble(&ok);
                if (!ok)
                {
                    errorStream << QString("ERROR: Invalid x scaling factor: %1").arg(value) << endl;
                    exit(1);
                }
            }

            double scaleY = 1.0;
            if (options->count("scale-y"))
            {
                value = options->value("scale-y").toString();
                scaleY = value.toDouble(&ok);
                if (!ok)
                {
                    errorStream << QString("ERROR: Invalid y scaling factor: %1").arg(value) << endl;
                    exit(1);
                }
            }

            subtitleProcessor->setFreeScale(scaleX, scaleY);
            subtitleProcessor->setApplyFreeScale(true);
            outStream << QString("OPTION: Set free scaling factors to %1, %2")
                         .arg(QString::number(scaleX, 'g', 6))
                         .arg(QString::number(scaleY, 'g', 6)) << endl;
        }

        if (options->count("alpha-crop"))
        {
            bool ok;
            value = options->value("alpha-crop").toString();
            ival = value.toInt(&ok);
            ival = ok ? ival : -1;
            if (ival < 0 || ival > 255)
            {
                errorStream << QString("ERROR: Illegal number range for alpha cropping threshold: %1")
                               .arg(value) << endl;
                exit(1);
            }
            else
            {
                subtitleProcessor->setAlphaCrop(ival);
            }
            outStream << QString("OPTION: Set alpha cropping threshold to %1").arg(value) << endl;
        }

        if (options->count("export-palette"))
        {
            subtitleProcessor->setWritePGCEditPal(true);
            outStream << QString("OPTION: Export target palette in PGCEDit text format") << endl;
        }
        if (options->count("no-export-palette"))
        {
            subtitleProcessor->setWritePGCEditPal(false);
        }

        if (options->count("fix-invisible"))
        {
            subtitleProcessor->setFixZeroAlpha(true);
            outStream << QString("OPTION: Fix zero alpha frame palette for SUB/IDX and SUP/IFO") << endl;
        }
        if (options->count("no-fix-invisible"))
        {
            subtitleProcessor->setFixZeroAlpha(false);
        }

        if (options->count("force-all"))
        {
            value = options->value("force-all").toString().toLower();
            if (value == "set" || value == "clear")
            {
                subtitleProcessor->setForceAll(value == "set" ? SetState::SET : SetState::CLEAR);
                outStream << QString("OPTION: Set forced state of all captions to: %1").arg(value) << endl;
            }
            else
            {
                errorStream << QString("Invalid set state: %1").arg(value) << endl;
                exit(1);
            }
        }

        if (srcFileNames.count() == 1 && trg == "")
        {
            fromCLI = true;
            loadPath = srcFileNames[0];
            return false;
        }
        else if (src == "" && trg == "")
        {
            fromCLI = true;
            loadPath = "";
            return false;
        }

        for (int fileNumber = 0; fileNumber < srcFileNames.size(); ++fileNumber)
        {
            subtitleProcessor->setActive(true);
            subtitleProcessor->setCliMode(true);
            src = srcFileNames[fileNumber];
            trg = trgFileNames[fileNumber];

            try
            {
                outStream << QString("\nConverting %1\n").arg(modes[(int)mode]) << endl;

                QFileInfo srcFileInfo(src);
                if (!srcFileInfo.exists())
                {
                    errorStream << QString("ERROR: File '%1' does not exist.").arg(QDir::toNativeSeparators(src)) << endl;
                    exit(1);
                }
                bool xml = srcFileInfo.completeSuffix().toLower() == "xml";
                bool idx = srcFileInfo.completeSuffix().toLower() == "idx";
                bool ifo = srcFileInfo.completeSuffix().toLower() == "ifo";
                QByteArray id = subtitleProcessor->getFileID(src, 4);
                StreamID sid = (id.isEmpty()) ? StreamID::UNKNOWN : subtitleProcessor->getStreamID(id);
                if (!idx && !xml && !ifo && sid == StreamID::UNKNOWN)
                {
                    errorStream << QString("File '%1' is not a supported subtitle stream.").arg(src) << endl;
                    exit(1);
                }

                QFileInfo fi, fs;
                QFileInfo trgFileInfo(trg);
                if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB)
                {
                    fi = QFileInfo(QString("%1/%2.idx").arg(trgFileInfo.absolutePath()).arg(trgFileInfo.completeBaseName()));
                    fs = QFileInfo(QString("%1/%2.sub").arg(trgFileInfo.absolutePath()).arg(trgFileInfo.completeBaseName()));
                }
                else
                {
                    fs = QFileInfo(QString("%1/%2.sup").arg(trgFileInfo.absolutePath()).arg(trgFileInfo.completeBaseName()));
                    fi = QFileInfo(QString("%1/%2.ifo").arg(srcFileInfo.absolutePath()).arg(srcFileInfo.completeBaseName()));
                    if (!fi.exists())
                    {
                        fi = fs; // ifo file doesn't exist set it to the same name as sup
                    }
                }
                if (fi.exists() || fs.exists())
                {
                    if ((fi.exists() && !fi.isWritable()) || (fs.exists() && !fs.isWritable()))
                    {
                        errorStream << QString("Target file '%1' is write protected.").arg(trg) << endl;
                        exit(1);
                    }
                }

                subtitleProcessor->setLoadPath(src);

                try
                {
                    if (xml || sid == StreamID::XML)
                    {
                        subtitleProcessor->readXml();
                    }
                    else if (idx || sid == StreamID::DVDSUB || sid == StreamID::IDX)
                    {
                        subtitleProcessor->readDVDSubStream(sid, true);
                    }
                    else if (ifo || sid == StreamID::IFO)
                    {
                        subtitleProcessor->readDVDSubStream(sid, false);
                    }
                    else if (sid == StreamID::BDSUP)
                    {
                        subtitleProcessor->readSup();
                    }
                    else
                    {
                        if (QFileInfo(QString("%1/%2.ifo").arg(srcFileInfo.absolutePath()).arg(srcFileInfo.completeBaseName())).exists())
                        {
                            subtitleProcessor->readDVDSubStream(sid, false);
                        }
                        else
                        {
                            try
                            {
                                subtitleProcessor->readDVDSubStream(sid, false);
                            }
                            catch(QString e)
                            {
                                outStream << "SUP file was not a valid DVD SUP file. Attempting to load as HD DVD SUP.\n";
                                subtitleProcessor->readSup();
                            }
                        }
                    }
                }
                catch (QString e)
                {
                    errorStream << e;
                    exit(1);
                }

                printWarnings(outStream);

                subtitleProcessor->scanSubtitles();

                if (subtitleProcessor->getMoveModeX() != MoveModeX::KEEP || subtitleProcessor->getMoveModeY() != MoveModeY::KEEP)
                {
                    subtitleProcessor->setCineBarFactor((1.0 - (16.0 / 9) / screenRatio) / 2.0);
                    subtitleProcessor->moveAllToBounds();
                }
                if (subtitleProcessor->getExportForced() && subtitleProcessor->getNumForcedFrames()==0)
                {
                    errorStream << "No forced subtitles found." << endl;
                    exit(1);
                }

                QVector<int> lumaThr = subtitleProcessor->getLuminanceThreshold();
                if (lumThr1 > 0)
                {
                    lumaThr.replace(0, lumThr1);
                }
                if (lumThr2 > 0)
                {
                    lumaThr.replace(1, lumThr2);
                }
                subtitleProcessor->setLuminanceThreshold(lumaThr);
                if (alphaThreshold > 0)
                {
                    subtitleProcessor->setAlphaThreshold(alphaThreshold);
                }
                if (langIdx > 0)
                {
                    subtitleProcessor->setLanguageIdx(langIdx);
                }
                subtitleProcessor->writeSub(trg);
            }
            catch(QString e)
            {
                errorStream << QString("ERROR: %1").arg(e) << endl;
                exit(1);
            }

            printWarnings(outStream);
            subtitleProcessor->exit();
        }
        outStream << QString("\nConversion of %1 file(s) finished\n").arg(QString::number(srcFileNames.size())) << endl;
    }
    exit(0);
}

void BDSup2Sub::Redirect_console()
{
#ifdef Q_OS_WIN
    int hCrtout,hCrterr;
    FILE *hfout, *hferr;

    AllocConsole();

    hCrtout=_open_osfhandle( (long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT );
    hfout=_fdopen(hCrtout, "w");
    *stdout= *hfout;
    setvbuf(stdout,NULL, _IONBF,0);

    hCrterr=_open_osfhandle( (long) GetStdHandle(STD_ERROR_HANDLE), _O_TEXT );
    hferr=_fdopen(hCrterr, "w");
    *stderr= *hferr;
    setvbuf(stderr,NULL, _IONBF,0);
#endif
}

void BDSup2Sub::showAboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void BDSup2Sub::print(const QString &message)
{
    ui->consoleOutput->insertPlainText(message);
    ui->consoleOutput->moveCursor(QTextCursor::End);
}

void BDSup2Sub::closeSubtitle()
{
    ui->subtitleNumberComboBox->blockSignals(true);
    ui->subtitleNumberComboBox->clear();
    ui->subtitleNumberComboBox->blockSignals(false);
    enableCoreComponents(false);
    ui->action_Load->setEnabled(true);
    updateRecentMenu();
    ui->paletteComboBox->setEnabled(false);
    ui->alphaThresholdComboBox->setEnabled(false);
    ui->hiMedThresholdComboBox->setEnabled(false);
    ui->medLowThresholdComboBox->setEnabled(false);
    ui->actionEdit_imported_DVD_Palette->setEnabled(false);
    ui->actionEdit_DVD_Frame_Palette->setEnabled(false);

    ui->subtitleImage->setImage(QImage(0, 0), 1, 1);
    ui->subtitleImage->update();
    ui->sourceImage->setImage(QImage(0, 0));
    ui->sourceImage->updateImage();
    ui->targetImage->setImage(QImage(0, 0));
    ui->targetImage->updateImage();

    ui->sourceInfoLabel->setText("");
    ui->targetInfoLabel->setText("");
}

void BDSup2Sub::updateRecentMenu()
{
    ui->menu_Recent_Files->setEnabled(false);
    if (subtitleProcessor->getRecentFiles().size() > 0)
    {
        ui->menu_Recent_Files->clear();
        for (auto recentFile : subtitleProcessor->getRecentFiles())
        {
            ui->menu_Recent_Files->addAction(QDir::toNativeSeparators(recentFile), this, SLOT(onRecentItemClicked()));
        }
        ui->menu_Recent_Files->setEnabled(true);
    }
}

void BDSup2Sub::onRecentItemClicked()
{
    if (QObject::sender() == 0)
    {
        return;
    }
    QAction* action = (QAction*)QObject::sender();
    QFileInfo info(action->text());
    if (!info.exists())
    {
        if (QMessageBox::warning(this, "Error!", "This file no longer exists. Remove from menu?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            subtitleProcessor->removeRecent(action->text());
            ui->menu_Recent_Files->removeAction(action);
        }
        return;
    }
    loadPath = info.absoluteFilePath();
    closeFile();
    connectSubtitleProcessor();
    loadSubtitleFile();
}

void BDSup2Sub::onEditPaneClicked(QMouseEvent *event)
{
    if (subtitleProcessor->getNumberOfFrames() > 0 && event->button() == Qt::LeftButton)
    {
        loadEditPane();
    }
}

void BDSup2Sub::loadEditPane()
{
    EditDialog editDialog(this, subtitleProcessor);
    editDialog.setIndex(subIndex);
    editDialog.exec();
    subIndex = editDialog.getIndex();
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshSrcFrame(subIndex);
    refreshTrgFrame(subIndex);
    ui->subtitleNumberComboBox->setCurrentIndex(subIndex);
}

void BDSup2Sub::loadHelpDialog()
{
    HelpDialog helpDialog(this);
    helpDialog.exec();
}

void BDSup2Sub::swapCrCb_toggled(bool checked)
{
    subtitleProcessor->setSwapCrCb(checked);
    if (subtitleProcessor->getNumberOfFrames() > 0)
    {
        try
        {
            subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
        }
        catch (QString e)
        {
            errorDialog(e);
            return;
        }
        refreshSrcFrame(subIndex);
        refreshTrgFrame(subIndex);
    }
}

void BDSup2Sub::fixInvisibleFrames_toggled(bool checked)
{
    subtitleProcessor->setFixZeroAlpha(checked);
}

void BDSup2Sub::verbatimOutput_toggled(bool checked)
{
    subtitleProcessor->setVerbatim(checked);
}

void BDSup2Sub::editDefaultDVDPalette_triggered()
{
    ColorDialog colorDialog(this, subtitleProcessor);
    QStringList colorNames = { "black", "white", "light gray", "dark gray",
                               "Color 1 light", "Color 1 dark",
                               "Color 2 light", "Color 2 dark",
                               "Color 3 light", "Color 3 dark",
                               "Color 4 light", "Color 4 dark",
                               "Color 5 light", "Color 5 dark",
                               "Color 6 light", "Color 6 dark"
                             };

    QVector<QColor> colors;
    QVector<QColor> defaultColors;

    for (int i = 0; i < colorNames.size(); ++i)
    {
        colors.push_back(subtitleProcessor->getCurrentDVDPalette().color(i));
        defaultColors.push_back(subtitleProcessor->getDefaultDVDPalette().color(i));
    }

    colorDialog.setParameters(colorNames, colors, defaultColors);
    colorDialog.setPath(colorPath);
    if (colorDialog.exec() != QDialog::Rejected)
    {
        colors = colorDialog.getColors();
        colorPath = colorDialog.getPath();
        for (int i = 0; i < colors.size(); ++i)
        {
            subtitleProcessor->getCurrentDVDPalette().setColor(i, colors[i]);
        }
        if (subtitleProcessor->getNumberOfFrames() > 0)
        {
            try
            {
                subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
            }
            catch (QString e)
            {
                errorDialog(e);
                return;
            }
            refreshTrgFrame(subIndex);
        }
    }
}

void BDSup2Sub::editImportedDVDPalette_triggered()
{
    ColorDialog colorDialog(this, subtitleProcessor);
    QStringList colorNames = { "Color 0", "Color 1", "Color 2", "Color 3",
                               "Color 4", "Color 5", "Color 6", "Color 7",
                               "Color 8", "Color 9", "Color 10", "Color 11",
                               "Color 12", "Color 13", "Color 14", "Color 15"
                             };

    QVector<QColor> colors;
    QVector<QColor> defaultColors;

    for (int i = 0; i < 16; ++i)
    {
        colors.push_back(subtitleProcessor->getCurrentSrcDVDPalette().color(i));
        defaultColors.push_back(subtitleProcessor->getDefaultSrcDVDPalette().color(i));
    }

    colorDialog.setParameters(colorNames, colors, defaultColors);
    colorDialog.setPath(colorPath);
    if (colorDialog.exec() != QDialog::Rejected)
    {
        colors = colorDialog.getColors();
        colorPath = colorDialog.getPath();
        Palette palette(colors.size(), true);
        for (int i = 0; i < colors.size(); ++i)
        {
            palette.setColor(i, colors[i]);
        }
        subtitleProcessor->setCurrentSrcDVDPalette(palette);
        if (subtitleProcessor->getNumberOfFrames() > 0)
        {
            try
            {
                subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
            }
            catch (QString e)
            {
                errorDialog(e);
                return;
            }
            refreshSrcFrame(subIndex);
            refreshTrgFrame(subIndex);
        }
    }
}

void BDSup2Sub::editDVDFramePalette_triggered()
{
    if (subtitleProcessor->getNumberOfFrames() > 0)
    {
        FramePaletteDialog framePaletteDialog(this, subtitleProcessor);
        framePaletteDialog.setIndex(subIndex);

        if (framePaletteDialog.exec() != QDialog::Rejected)
        {
            try
            {
                subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
            }
            catch (QString e)
            {
                errorDialog(e);
                return;
            }
            refreshSrcFrame(subIndex);
            refreshTrgFrame(subIndex);
        }
    }
}

void BDSup2Sub::moveAllCaptions_triggered()
{
    MoveDialog moveDialog(this, subtitleProcessor);
    moveDialog.setIndex(subIndex);
    if (moveDialog.exec() != QDialog::Rejected)
    {
        if (subtitleProcessor->getMoveCaptions())
        {
            subtitleProcessor->moveAll();
        }
        subIndex = moveDialog.getIndex();
        ui->subtitleImage->setScreenRatio(moveDialog.getTrgRatio());
        try
        {
            subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
        }
        catch (QString e)
        {
            errorDialog(e);
            return;
        }
        refreshSrcFrame(subIndex);
        refreshTrgFrame(subIndex);
        ui->subtitleNumberComboBox->setCurrentIndex(subIndex);
    }
}

void BDSup2Sub::resetCropOffset_triggered()
{
    subtitleProcessor->setCropOfsY(0);
    ui->subtitleImage->setCropOfsY(0);
    ui->subtitleImage->update();
}

void BDSup2Sub::refreshSrcFrame(int index)
{
    ui->sourceImage->setImage(subtitleProcessor->getSrcImage());
    ui->sourceImage->updateImage();
    ui->sourceInfoLabel->setText(subtitleProcessor->getSrcInfoStr(index));
}

void BDSup2Sub::refreshTrgFrame(int index)
{
    ui->subtitleImage->setDimension(subtitleProcessor->getTrgWidth(index), subtitleProcessor->getTrgHeight(index));
    ui->subtitleImage->setOffsets(subtitleProcessor->getTrgOfsX(index), subtitleProcessor->getTrgOfsY(index));
    ui->subtitleImage->setCropOfsY(subtitleProcessor->getCropOfsY());
    QImage image = subtitleProcessor->getTrgImage();
    ui->subtitleImage->setImage(image.copy(image.rect()), subtitleProcessor->getTrgImgWidth(index), subtitleProcessor->getTrgImgHeight(index));
    ui->subtitleImage->setExcluded(subtitleProcessor->getTrgExcluded(index));
    ui->targetImage->setImage(image.copy(image.rect()));
    ui->targetImage->updateImage();
    ui->targetInfoLabel->setText(subtitleProcessor->getTrgInfoStr(index));
}

void BDSup2Sub::on_subtitleNumberComboBox_currentIndexChanged(int index)
{
    subIndex = index;
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshSrcFrame(subIndex);
    refreshTrgFrame(subIndex);
}

void BDSup2Sub::openConversionSettings()
{
    Resolution oldResolution = subtitleProcessor->getOutputResolution();
    double fpsTrgOld = subtitleProcessor->getFPSTrg();
    bool changeFpsOld = subtitleProcessor->getConvertFPS();
    int delayOld = subtitleProcessor->getDelayPTS();
    double fsXOld = 1.0, fsYOld = 1.0;
    if (subtitleProcessor->getApplyFreeScale())
    {
        fsXOld = subtitleProcessor->getFreeScaleX();
        fsYOld = subtitleProcessor->getFreeScaleY();
    }

    ConversionDialog conversionDialog(this, subtitleProcessor, settings);
    conversionDialog.enableOptionMove(subtitleProcessor->getMoveCaptions());
    if (conversionDialog.exec() != QDialog::Rejected && subtitleProcessor->getNumberOfFrames() > 0)
    {
        subtitleProcessor->reScanSubtitles(oldResolution, fpsTrgOld, delayOld, changeFpsOld, fsXOld, fsYOld);
        try
        {
            subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
        }
        catch (QString e)
        {
            errorDialog(e);
            return;
        }
        refreshTrgFrame(subIndex);
    }
}

void BDSup2Sub::on_paletteComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setPaletteMode((PaletteMode)index);
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshTrgFrame(subIndex);
}

void BDSup2Sub::on_filterComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setScalingFilter((ScalingFilters)index);
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshTrgFrame(subIndex);
}

void BDSup2Sub::on_hiMedThresholdComboBox_currentIndexChanged(int index)
{
    int idx = index;
    QVector<int> lumaThreshold = subtitleProcessor->getLuminanceThreshold();

    if (idx <= lumaThreshold[1])
    {
        idx = lumaThreshold[1] + 1;
    }

    if (idx < 0)
    {
        idx = 0;
    }
    if (idx > 255)
    {
        idx = 255;
    }

    lumaThreshold[0] = idx;
    subtitleProcessor->setLuminanceThreshold(lumaThreshold);
    ui->hiMedThresholdComboBox->blockSignals(true);
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshTrgFrame(subIndex);
    ui->hiMedThresholdComboBox->blockSignals(false);
    ui->hiMedThresholdComboBox->setCurrentIndex(idx);
}

void BDSup2Sub::on_medLowThresholdComboBox_currentIndexChanged(int index)
{
    int idx = index;
    QVector<int> lumaThreshold = subtitleProcessor->getLuminanceThreshold();
    if (idx >= lumaThreshold[0])
    {
        idx = lumaThreshold[0] - 1;
    }

    if (idx < 0)
    {
        idx = 0;
    }
    if (idx > 255)
    {
        idx = 255;
    }

    lumaThreshold[1] = idx;
    subtitleProcessor->setLuminanceThreshold(lumaThreshold);
    ui->medLowThresholdComboBox->blockSignals(true);
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshTrgFrame(subIndex);
    ui->medLowThresholdComboBox->blockSignals(false);
    ui->medLowThresholdComboBox->setCurrentIndex(idx);
}

void BDSup2Sub::on_alphaThresholdComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setAlphaThreshold(index);
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshTrgFrame(subIndex);
}

void BDSup2Sub::on_outputFormatComboBox_currentIndexChanged(int index)
{
    OutputMode outMode = (OutputMode)index;
    subtitleProcessor->setOutputMode(outMode);
    try
    {
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    }
    catch (QString e)
    {
        errorDialog(e);
        return;
    }
    refreshTrgFrame(subIndex);

    if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getInputMode() == InputMode::SUPIFO)
    {
        enableVobSubComponents(true);
    }
    else
    {
        enableVobSubComponents(false);
    }
}

void BDSup2Sub::on_subtitleLanguageComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setIdxToRead(index);

    connectSubtitleProcessor();

    QThread *workerThread = new QThread;
    subtitleProcessor->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(readSubtitleStream()));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished(QString)), workerThread, SLOT(deleteLater()));

    subtitleProcessor->setActive(true);
    workerThread->start();
}
