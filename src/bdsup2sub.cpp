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

#ifdef Q_WS_WIN
#include "windows.h"
#include "io.h"
#include "fcntl.h"
#endif

#include <QMessageBox>
#include <QPixmap>
#include <QFileInfoList>
#include <QFileInfo>
#include <QSettings>

BDSup2Sub::BDSup2Sub(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDSup2Sub)
{
    ui->setupUi(this);

    //Rename INI file to name matching the new program name
    QString oldIniFilePath = QString("%1/%2").arg(QDir::currentPath()).arg(oldIniName);
    QString newIniFilePath = QString("%1/%2").arg(QDir::currentPath()).arg(iniName);
    QFileInfo oldIniFileInfo = QFileInfo(oldIniFilePath);
    if (oldIniFileInfo.exists())
    {
        QDir dir = oldIniFileInfo.absoluteDir();
        dir.rename(oldIniFilePath, newIniFilePath);
        settings = new QSettings(newIniFilePath, QSettings::IniFormat);

        //Fix mangled Windows path names written by Java in INI files imported from original BDSup2Sub
#ifdef Q_WS_WIN
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

    QRect geometry = this->geometry();
    int w = settings->value("frameWidth", 800).toInt();
    int h = settings->value("frameHeight", 600).toInt();
    setMinimumWidth(w);
    setMinimumHeight(h);
    geometry.setWidth(w);
    geometry.setHeight(h);

    QPoint center = QDesktopWidget().availableGeometry().center();
    center.setX(center.x() - (geometry.width() / 2));
    center.setY(center.y() - (geometry.height() / 2));
    geometry.setX(settings->value("framePosX", center.x()).toInt());
    geometry.setY(settings->value("framePosY", center.y()).toInt());

    setGeometry(geometry);

    loadPath = settings->value("loadPath", "").toString();
    colorPath = settings->value("colorPath", "").toString();

    okBackground = new QPalette(ui->subtitleNumberComboBox->palette());
    errorBackground = new QPalette();
    errorBackground->setColor(QPalette::Base, QColor(0xffe1acac));

    this->setWindowTitle(progNameVer);

    QFont font("Monospace", 9);
    font.setStyleHint(QFont::TypeWriter);
    ui->consoleOutput->setFont(font);

    ui->consoleOutput->insertPlainText(progNameVer + " - a converter from Blu-Ray/HD-DVD SUP to DVD SUB/IDX and more\n");
    ui->consoleOutput->insertPlainText(authorDate + "\n");
    ui->consoleOutput->insertPlainText("Official thread at Doom9: http://forum.doom9.org/showthread.php?t=145277\n\n");

    subtitleProcessor = new SubtitleProcessor(this, settings);
    updateRecentMenu();

    fillComboBoxes();

    ui->action_Verbatim_Output->setChecked(subtitleProcessor->getVerbatim());
    ui->action_Fix_invisible_frames->setChecked(subtitleProcessor->getFixZeroAlpha());

    connect(ui->action_Load, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->action_Save_Export, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->action_Close, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(ui->action_Conversion_Settings, SIGNAL(triggered()), this, SLOT(openConversionSettings()));
    connect(ui->subtitleImage, SIGNAL(onMouseClicked(QMouseEvent*)), this, SLOT(onEditPaneClicked(QMouseEvent*)));
    connect(ui->action_Edit_Frame, SIGNAL(triggered()), this, SLOT(loadEditPane()));
    //connect(ui->action_Help, SIGNAL(triggered()), this, SLOT(loadHelpDialog()));
    connect(ui->action_Swap_Cr_Cb, SIGNAL(toggled(bool)), this, SLOT(swapCrCb_toggled(bool)));
    connect(ui->action_Verbatim_Output, SIGNAL(toggled(bool)), this, SLOT(verbatimOutput_toggled(bool)));
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

BDSup2Sub::~BDSup2Sub()
{
    delete ui;
}

void BDSup2Sub::closeEvent(QCloseEvent *event)
{
    if (!isMaximized())
    {
        QRect geometry = this->geometry();
        settings->setValue("frameWidth", QVariant(geometry.width()));
        settings->setValue("frameHeight", QVariant(geometry.height()));
        // store frame pos
        settings->setValue("framePosX", QVariant(geometry.x()));
        settings->setValue("framePosY", QVariant(geometry.y()));
    }
    settings->setValue("loadPath", QVariant(loadPath));
    settings->setValue("colorPath", QVariant(colorPath));

    subtitleProcessor->exit();
    QMainWindow::closeEvent(event);
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
        ui->subtitleNumberComboBox->clear();
        ui->subtitleNumberComboBox->blockSignals(true);
        subtitleNumberValidator = new QIntValidator(1, num, this);
        for (int i = 1; i <= num; ++i)
        {
            ui->subtitleNumberComboBox->addItem(QString::number(i));
        }
        ui->subtitleNumberComboBox->setValidator(subtitleNumberValidator);
        ui->subtitleNumberComboBox->setCurrentIndex(subIndex);
        ui->subtitleNumberComboBox->blockSignals(false);
        ui->alphaThresholdComboBox->blockSignals(true);
        ui->medLowThresholdComboBox->blockSignals(true);
        ui->hiMedThresholdComboBox->blockSignals(true);
        ui->alphaThresholdComboBox->setCurrentIndex(subtitleProcessor->getAlphaThreshold());
        ui->hiMedThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[0]);
        ui->medLowThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[1]);
        ui->alphaThresholdComboBox->blockSignals(false);
        ui->medLowThresholdComboBox->blockSignals(false);
        ui->hiMedThresholdComboBox->blockSignals(false);

        if (subtitleProcessor->getCropOfsY() > 0)
        {
            if (QMessageBox::question(this, "", "Reset Crop Offset?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            {
                subtitleProcessor->setCropOfsY(0);
            }
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
                connect(subtitleProcessor, SIGNAL(moveAllFinished(QString)), workerThread, SLOT(quit()));
                connect(subtitleProcessor, SIGNAL(moveAllFinished(QString)), workerThread, SLOT(deleteLater()));

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
        ui->paletteComboBox->setCurrentIndex((int)PaletteMode::NEW);
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
                                                    loadPath.isEmpty() ? QDir::currentPath() : QFileInfo(loadPath).absolutePath(),
                                                    filter,
                                                    &selectedFilter
                                                    );

    if (fileName.isNull() || fileName.isEmpty() || !QFile(fileName).exists()) return;

    loadPath = fileName;

    loadSubtitleFile();
}

void BDSup2Sub::saveFile()
{
    QString path = savePath + "/" + saveFileName + "_exp.";
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
            fi = savePath + "/" + fileInfo.completeBaseName() + ".idx";
            fs = savePath + "/" + fileInfo.completeBaseName() + ".sub";
        }
        else if (ui->outputFormatComboBox->currentText().contains("IFO"))
        {
            fi = savePath + "/" + fileInfo.completeBaseName() + ".ifo";
            fs = savePath + "/" + fileInfo.completeBaseName() + ".sup";
        }
        if (ui->outputFormatComboBox->currentText().contains("BD"))
        {
            fs = savePath + "/" + fileInfo.completeBaseName() + ".sup";
            fi = fs;
        }
        if (ui->outputFormatComboBox->currentText().contains("XML"))
        {
            fs = savePath + "/" + fileInfo.completeBaseName() + ".xml";
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
        connect(subtitleProcessor, SIGNAL(writingSubtitleFinished(QString)), workerThread, SLOT(quit()));
        connect(subtitleProcessor, SIGNAL(writingSubtitleFinished(QString)), workerThread, SLOT(deleteLater()));

        workerThread->start();
    }
}

void BDSup2Sub::closeFile()
{
    closeSubtitle();
    subtitleProcessor->close();
}

void BDSup2Sub::loadSubtitleFile()
{
    QFileInfo fileInfo(loadPath);
    QString extension = fileInfo.suffix().toLower();
    bool isXML(extension == "xml"), isIDX(extension == "idx"), isIFO(extension == "ifo");

    QByteArray id = subtitleProcessor->getFileID(loadPath, 4);
    StreamID streamID = (id.isEmpty()) ? StreamID::UNKNOWN : subtitleProcessor->getStreamID(id);

    if (!isXML && !isIDX && !isIFO && streamID == StreamID::UNKNOWN)
    {
        QMessageBox::warning(this, "Wrong format!", "This is not a supported SUP stream");
        return;
    }

    subIndex = 0;
    saveFileName = QFileInfo(loadPath).completeBaseName();
    savePath = QFileInfo(loadPath).absolutePath();

    enableCoreComponents(false);
    enableVobSubComponents(false);

    QThread *workerThread = new QThread;
    subtitleProcessor->setLoadPath(loadPath);
    subtitleProcessor->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(readSubtitleStream()));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished(QString)), workerThread, SLOT(quit()));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished(QString)), workerThread, SLOT(deleteLater()));

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

void BDSup2Sub::printWarnings()
{
    QString message = getWarningMessage();
    if (!message.isNull() || !message.isEmpty())
    {
        fprintf(stdout, QString(message + "\n").toAscii());
    }
}

bool BDSup2Sub::execCLI()
{
    Redirect_console();

    outStream = new QTextStream(stdout);
    errorStream = new QTextStream(stderr);

    QStringList args = QCoreApplication::arguments();
    QStringList al(args[0]);

    if (args.size() == 2)
    {
        int extCnt = 0;
        int pos = 0;
        QString a = args[1].toLower();
        do
        {
            int p = a.indexOf(".sup", pos);
            if (p == -1)
            {
                p = a.indexOf(".sub", pos);
            }
            if (p == -1)
            {
                p = a.indexOf(".idx", pos);
            }
            if (p == -1)
            {
                p = a.indexOf(".xml", pos);
            }
            if (p != -1)
            {
                pos = p + 4;
                extCnt++;
            }
            else
            {
                pos = -1;
            }
        } while (pos != -1 && pos < a.length());

        bool isValidFile = false;
        QString testFileName = args[1].trimmed();

        if (QFile(testFileName).exists())
        {
            isValidFile = true;
        }
        else
        {
            QFile testFile(testFileName);
            if (testFile.open(QIODevice::WriteOnly))
            {
                isValidFile = true;
                testFile.remove();
            }
            else
            {
                isValidFile = false;
            }
        }

        if (!isValidFile && (args[1].indexOf('/') >= 0 ||
           (args[1].indexOf(' ') >= 0 && extCnt > 1)))
        {
            bool inside = false;
            QString s = args[1].trimmed();
            QString sb;

            for (int i = 0; i < s.length(); ++i)
            {
                char c = s.at(i).toAscii();
                switch (c)
                {
                case ' ':
                {
                    if (inside)
                    {
                        sb.append(" ");
                    }
                    else
                    {
                        if (sb.length() > 0)
                        {
                            al.push_back(sb);
                        }
                        sb = QString("");
                        for (int j = i + 1; j < s.length(); ++j)
                        {
                            c = s.at(j).toAscii();
                            if (c != ' ')
                            {
                                i = j - 1;
                                break;
                            }
                        }
                    }
                } break;
                case '\'':
                {
                    inside = !inside;
                } break;
                default:
                {
                    sb.append(c);
                }
                }
            }
            if (inside)
            {
                fprintf(stderr, QString("ERROR: Missing closing single quote").toAscii());
                exit(1);
            }
            if (sb.length() > 0)
            {
                al.push_back(sb);
            }
            args = al;
        }
    }

    if (args.size() == 2 && (args[1] == "/?" || args[1].toLower() == "/help"))
    {
        // output help
        fprintf(stdout, (QString("%1 - %2\n").arg(progNameVer).arg(authorDate)).toAscii());
        fprintf(stdout, "Syntax:\n");
        fprintf(stdout, "BDSup2Sub <in> <out> [options]\n");
        fprintf(stdout, "Options:\n");
        fprintf(stdout, "    /res:<n>         : set resolution to 480, 576, 720 or 1080 - default 576\n");
        fprintf(stdout, "                       Predefined values: keep, ntsc=480, pal=576, 1440x1080\n");
        fprintf(stdout, "    /fps:<t>         : synchronize target frame rate to <t> - default: auto\n");
        fprintf(stdout, "                       Predefined values; 24p=23.976, 25p=25, 30p=29.967\n");
        fprintf(stdout, "                       /fps:keep preserves the source fps (for BD&XML)\n");
        fprintf(stdout, "    /fps:<s>,<t>     : convert framerate from <s> to <t>\n");
        fprintf(stdout, "                       Predefined values; 24p=23.976, 25p=25, 30p=29.967\n");
        fprintf(stdout, "                       /fps:auto,<t> detects source frame rate automatically\n");
        fprintf(stdout, "    /dly:<t>         : set delay in ms - default: 0.0\n");
        fprintf(stdout, "    /filter:<f>      : set scaling filter - default: bilinear\n");
        fprintf(stdout, "                       Supported values: bilinear, triangle, bicubic, bell,\n");
        fprintf(stdout, "                       b-spline, hermite, lanczos3, mitchell\n");
        fprintf(stdout, "    /palmode:<s>     : palette mode: keep, create, dither - default: create\n");
        fprintf(stdout, "    /tmin:<t>        : set minimum display time in ms - default: 500\n");
        fprintf(stdout, "    /tmerge:<t>      : set max time diff for merging subs in ms - default: 200\n");
        fprintf(stdout, "    /movin:<r>[,<o>] : move captions inside screen ratio <r>, +/- offset <o>\n");
        fprintf(stdout, "    /movout:<r>[,<o>]: move captions outside screen ratio <r>, +/- offset <o>\n");
        fprintf(stdout, "    /movex:<t>[,<o>] : move captions horizontally.<t> may be left,right,center\n");
        fprintf(stdout, "                       +/- optional offset <o> (only if moving left or right)\n");
        fprintf(stdout, "    /cropy:<n>       : crop the upper/lower n lines - default: 0\n");
        fprintf(stdout, "    /acrop:<n>       : set alpha cropping threshold - default: 10\n");
        fprintf(stdout, "    /scale:<x>,<y>   : scale captions with free factors - default: 1.0,1.0\n");
        fprintf(stdout, "    /exppal[+/-]     : export target palette in PGCEdit format - default: off\n");
        fprintf(stdout, "    /forced[+/-]     : export only forced subtitles - default: off (export all)\n");
        fprintf(stdout, "    /forceall[+/-]   : set/clear forced flag for all subs - default: off (keep)\n");
        fprintf(stdout, "    /swap[+/-]       : swap Cr/Cb components - default: off (don't swap)\n");
        fprintf(stdout, "    /fixinv[+/-]     : fix zero alpha frame palette - default: off (don't fix)\n");
        fprintf(stdout, "    /verbatim[+/-]   : switch on verbatim console output mode - default: off\n");
        fprintf(stdout, "Options only for SUB/IDX or SUP/IFO as target:\n");
        fprintf(stdout, "    /atr:<n>         : set alpha threshold 0..255 - default 80\n");
        fprintf(stdout, "    /ltr1:<n>        : set lum lo/mid threshold 0..255 - default auto\n");
        fprintf(stdout, "    /ltr2:<n>        : set lum mid/hi threshold 0..255 - default auto\n");
        fprintf(stdout, "    /lang:<s>        : set language to string <s> - default: de (only SUB/IDX)\n");
        fprintf(stdout, "    /pal:<s>         : load palette file <s> - default: use builtin palette\n");
        fprintf(stdout, "\nNote:\n");
        fprintf(stdout, "Boolean parameters like \"verbatim\" are switched off with a trailing \"-\".\n");
        fprintf(stdout, "\nWildcard support:\n");
        fprintf(stdout, "To use wildcards, enclose the whole parameter string in double quotes.\n");
        fprintf(stdout, "For filenames containing spaces, use single quotes around the file name.\n");
        fprintf(stdout, "Use \"*\" for any character and \"?\" for one character in the source name\n");
        fprintf(stdout, "Use exactly one \"*\" in the target file name.\n");
        fprintf(stdout, "Example:\n");
        fprintf(stdout, "BDSup2Sub \"'movie* 1?.sup' dvd_*.sub /res:720 /fps:25p\"\n");
    }
    else
    {
        // analyze parameters
        QString cmdLine = "";
        for (int i = 1; i < args.size(); ++i)
        {
            cmdLine += args[i] + " ";
        }
        fprintf(stdout, QString("\nCommand line:\n" + args[0]
                                + " " + cmdLine + "\n").toAscii());

        // parse parameters
        QString src = "";
        QString trg = "";
        int alphaThr = subtitleProcessor->getAlphaThreshold();
        int lumThr1 = -1;
        int lumThr2 = -1;
        int langIdx = -1;
        Resolution r = Resolution::PAL;
        OutputMode mode = (OutputMode)0;
        bool defineFPStrg = false;
        double screenRatio = -1;
        for (int i = 1; i < args.size() ; ++i)
        {
            QString a = args[i];

            // detect source and target
#ifdef Q_WS_WIN
            if (a.at(0) != QChar('/'))
            {
#endif
                if (src.isEmpty())
                {
                    src = a;
                    continue;
                }
                else if (trg.isEmpty())
                {
                    trg = a;

                    QString ext = QFileInfo(trg).suffix();
                    if (ext.isEmpty())
                    {
                        fprintf(stderr, QString("ERROR: No extension given for target " + trg).toAscii());
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
                        fprintf(stderr, QString("ERROR: Unknown extension of target " + trg).toAscii());
                        exit(1);
                    }
                    subtitleProcessor->setOutputMode(mode);
                    continue;
                }
#ifdef Q_WS_WIN
            }
#endif

            bool switchOn = true;

            // analyze normal parameters
            if (a.length() < 4 || a.at(0) != QChar('/'))
            {
                fprintf(stderr, QString("ERROR: Illegal argument: " + a).toAscii());
                exit(1);
            }
            int pos = a.indexOf(':');
            QString val;
            if (pos > -1)
            {
                val = a.mid(pos + 1, a.length());
                a = a.mid(1, pos);
            }
            else
            {
                val = "";
                a = a.mid(1);
                // check +/- at end of parameter
                int last = a.length() - 1;
                if (a.indexOf('+') == last)
                {
                    a = a.mid(0, last);
                }
                else if (a.indexOf('-') == last)
                {
                    a = a.mid(0, last);
                    switchOn = false;
                }
            }

            QString strSwitchOn;
            if (switchOn)
            {
                strSwitchOn = "ON";
            }
            else
            {
                strSwitchOn = "OFF";
            }

            bool ok;
            int ival = val.toInt(&ok);
            ival = ok ? ival : -1;

            Parameters p = (Parameters)params.indexOf(a);

            switch ((int)p)
            {
            case (int)Parameters::ALPHATHR:
            {
                // alpha threshold for SUB/IDX conversion
                if (ival <0 || ival > 255)
                {
                    fprintf(stderr, (QString("ERROR: Illegal number range for alpha threshold: %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }
                else
                {
                    alphaThr = ival;
                }
                fprintf(stdout, (QString("OPTION: set alpha threshold to %1")
                                 .arg(QString::number(ival))).toAscii());
            } break;
            case (int)Parameters::LUMTHR1:
            {
                // luminance threshold low-med for SUB/IDX conversion
                if (ival <0 || ival > 255)
                {
                    fprintf(stderr, (QString("ERROR: Illegal number range for luminance: %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }
                else
                {
                    lumThr1 = ival;
                }
                fprintf(stdout, (QString("OPTION: set low/mid luminance threshold to %1")
                                 .arg(QString::number(ival))).toAscii());
            } break;
            case (int)Parameters::LUMTHR2:
            {
                // luminance threshold med-high for SUB/IDX conversion
                if (ival <0 || ival > 255)
                {
                    fprintf(stderr, (QString("ERROR: Illegal number range for luminance: %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }
                else
                {
                    lumThr2 = ival;
                }
                fprintf(stdout, (QString("OPTION: set mid/high luminance threshold to %1")
                                 .arg(QString::number(ival))).toAscii());
            } break;
            case (int)Parameters::RESOLUTION:
            {
                // resolution for export
                if (val.toLower() == "keep")
                {
                    subtitleProcessor->setConvertResolution(false);
                }
                else
                {
                    subtitleProcessor->setConvertResolution(true);
                    if (val.toLower() == "pal" || ival == 576)
                    {
                        r = Resolution::PAL;
                        if (!defineFPStrg)
                        {
                            subtitleProcessor->setFPSTrg(FPS_PAL);
                        }
                    }
                    else if (val.toLower() == "ntsc" || ival == 480)
                    {
                        r = Resolution::NTSC;
                        if (!defineFPStrg)
                        {
                            subtitleProcessor->setFPSTrg(FPS_NTSC);
                        }
                    }
                    else if (val.toLower() == "720p" || ival == 720)
                    {
                        r = Resolution::HD_720;
                        if (!defineFPStrg)
                        {
                            subtitleProcessor->setFPSTrg(FPS_24P);
                        }
                    }
                    else if (val.toLower() == "1440x1080")
                    {
                        r = Resolution::HD_1440x1080;
                        if (!defineFPStrg)
                        {
                            subtitleProcessor->setFPSTrg(FPS_24P);
                        }
                    }
                    else if (val.toLower() == "1080p" || ival == 1080)
                    {
                        r = Resolution::HD_1080;
                        if (!defineFPStrg)
                        {
                            subtitleProcessor->setFPSTrg(FPS_24P);
                        }
                    }
                    else
                    {
                        fprintf(stderr, (QString("ERROR: Illegal resolution: %1")
                                         .arg(val)).toAscii());
                        exit(1);
                    }
                }
                fprintf(stdout, (QString("OPTION: set resolution to %1")
                                 .arg(subtitleProcessor->getResolutionName(r))).toAscii());
            } break;
            case (int)Parameters::LANGUAGE:
            {
                // language used for SUB/IDX export
                langIdx = -1;
                for (int l = 0; l < subtitleProcessor->getLanguages().size(); ++l)
                {
                    if (subtitleProcessor->getLanguages()[l][1] == val)
                    {
                        langIdx = l;
                        break;
                    }
                }
                if (langIdx == -1)
                {
                    fprintf(stderr, (QString("ERROR: Unknown language %1").arg(val)).toAscii());
                    fprintf(stderr, "Use one of the following 2 character codes:");
                    for (int l = 0; l < subtitleProcessor->getLanguages().size(); ++l)
                    {
                        fprintf(stderr, (QString("    %1 - %2")
                                         .arg(subtitleProcessor->getLanguages()[l][1])
                                         .arg(subtitleProcessor->getLanguages()[l][0])).toAscii());
                    }
                    exit(1);
                }
                fprintf(stdout, (QString("OPTION: set language to %1 (%2)")
                                 .arg(subtitleProcessor->getLanguages()[langIdx][0])
                                 .arg(subtitleProcessor->getLanguages()[langIdx][1])).toAscii());
            } break;
            case (int)Parameters::PALETTE:
            {
                // load color profile for for SUB/IDX conversion
                QFileInfo f(val);
                if (f.exists())
                {
                    QByteArray id = subtitleProcessor->getFileID(val, 4);
                    if (id.isEmpty() || (char)id[0] != 0x23 || (char)id[1] != 0x43 ||
                                        (char)id[2] != 0x4F || (char)id[3] != 0x4C) //#COL
                    {
                        fprintf(stderr, (QString("ERROR: No valid palette file: %1").arg(val)).toAscii());
                        exit(1);
                    }
                }
                else
                {
                    fprintf(stderr, (QString("ERROR: Palette file not found: %1").arg(val)).toAscii());
                    exit(1);
                }
                QSettings colorSettings(val, QSettings::IniFormat);
                for (int c = 0; c < 15; ++c)
                {
                    QString s = settings->value(QString("Color_%1").arg(QString::number(c)), QVariant("0,0,0")).toString();
                    QStringList sp = s.split(",");
                    if (sp.size() >= 3)
                    {
                        int red = sp[0].trimmed().toInt() & 0xff;
                        int green = sp[1].trimmed().toInt() & 0xff;
                        int blue = sp[2].trimmed().toInt() & 0xff;
                        subtitleProcessor->getCurrentDVDPalette()->setColor(c + 1, QColor(red, green, blue, 0));
                    }
                }
                fprintf(stdout, (QString("OPTION: loaded palette from %1").arg(val)).toAscii());
            } break;
            case (int)Parameters::FORCED:
            {
                // export only forced subtitles (when converting from BD-SUP)
                subtitleProcessor->setExportForced(switchOn);
                fprintf(stdout, (QString("OPTION: export only forced subtitles: %1").arg(strSwitchOn)).toAscii());
            } break;
            case (int)Parameters::SWAP_CR_CB:
            {
                // export only forced subtitles (when converting from BD-SUP)
                subtitleProcessor->setSwapCrCb(switchOn);
                fprintf(stdout, (QString("OPTION: swap Cr/Cb components: %1").arg(strSwitchOn)).toAscii());
            } break;
            case (int)Parameters::FPS:
            {
                // set target (and source) frame rate
                double fpsSrc, fpsTrg;
                pos = val.indexOf(',');
                if (pos > 0)
                {
                    bool autoFPS;
                    // source and target: frame rate conversion
                    QString srcStr = val.mid(0, pos).trimmed();
                    if (srcStr.toLower() == "auto")
                    {
                        // leave default value
                        autoFPS = true;
                        fpsSrc = 0; // stub to avoid undefined warning
                    }
                    else
                    {
                        autoFPS = false;
                        fpsSrc = subtitleProcessor->getFPS(srcStr);
                        if (fpsSrc <= 0)
                        {
                            fprintf(stderr, (QString("ERROR: invalid source framerate: %1")
                                             .arg(srcStr)).toAscii());
                            exit(1);
                        }
                        subtitleProcessor->setFPSSrc(fpsSrc);
                    }
                    fpsTrg = subtitleProcessor->getFPS(val.mid(pos + 1));
                    if (fpsTrg <= 0)
                    {
                        fprintf(stderr, (QString("ERROR: invalid target value: %1")
                                         .arg(val.mid(pos + 1))).toAscii());
                        exit(1);
                    }
                    if (!autoFPS)
                    {
                        subtitleProcessor->setFPSTrg(fpsTrg);
                    }

                    subtitleProcessor->setConvertFPS(true);

                    fprintf(stdout, (QString("OPTION: convert framerate from %1fps to %2fps")
                                     .arg(autoFPS ? "<auto>" : QString::number(fpsSrc, 'g', 6))
                                     .arg(QString::number(fpsTrg, 'g', 6))).toAscii());

                    defineFPStrg = true;
                }
                else
                {
                    // only target: frame rate synchronization
                    if (val.toLower() == "keep")
                    {
                        subtitleProcessor->setKeepFps(true);
                        fprintf(stdout, "OPTION: use source fps as target fps");
                    }
                    else
                    {
                        fpsTrg = subtitleProcessor->getFPS(val);
                        if (fpsTrg <= 0)
                        {
                            fprintf(stderr, (QString("ERROR: invalid target framerate: %1").arg(val)).toAscii());
                            exit(1);
                        }
                        subtitleProcessor->setFPSTrg(fpsTrg);

                        fprintf(stdout, (QString("OPTION: synchronize target framerate to %1fps")
                                         .arg(QString::number(fpsTrg, 'g', 6))).toAscii());

                        defineFPStrg = true;
                    }
                }
            } break;
            case (int)Parameters::DELAY:
            {
                // add a delay
                double delay = 0;
                bool ok;
                delay = val.trimmed().toDouble(&ok) * 90.0;
                if (!ok)
                {
                    fprintf(stderr, (QString("ERROR: Illegal delay value: %1").arg(val)).toAscii());
                    exit(1);
                }
                int delayPTS = (int)subtitleProcessor->syncTimePTS((long)delay, subtitleProcessor->getFPSTrg());
                subtitleProcessor->setDelayPTS(delayPTS);
                fprintf(stdout, (QString("OPTION: set delay to %1")
                                 .arg(QString::number(delayPTS / 90.0, 'g', 6))).toAscii());
            } break;
            case (int)Parameters::MIN_TIME:
            {
                // set minimum duration
                double t = 0;
                bool ok;
                t = val.trimmed().toDouble(&ok) * 90.0;
                if (!ok)
                {
                    fprintf(stderr, (QString("ERROR: Illegal value for minimum display time: %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }
                int tMin = (int)subtitleProcessor->syncTimePTS((long)t, subtitleProcessor->getFPSTrg());
                subtitleProcessor->setMinTimePTS(tMin);
                subtitleProcessor->setFixShortFrames(true);
                fprintf(stdout, (QString("OPTION: set delay to %1")
                                 .arg(QString::number(tMin / 90.0, 'g', 6))).toAscii());
            } break;
            case (int)Parameters::MOVE_INSIDE:
            case (int)Parameters::MOVE_OUTSIDE:
            {
                // move captions
                QString sm;
                if (p == Parameters::MOVE_OUTSIDE)
                {
                    subtitleProcessor->setMoveModeY(MoveModeY::OUTSIDE);
                    sm = "outside";
                }
                else
                {
                    subtitleProcessor->setMoveModeY(MoveModeY::INSIDE);
                    sm = "inside";
                }
                QString ratio;
                pos = val.indexOf(',');
                if (pos > 0)
                {
                    ratio = val.mid(0, pos);
                }
                else
                {
                    ratio = val;
                }
                bool ok;
                screenRatio = ratio.toDouble(&ok);
                screenRatio = ok ? screenRatio : -1;
                if (screenRatio <= (16.0/9))
                {
                    fprintf(stderr, (QString("ERROR: invalid screen ratio: %1")
                                     .arg(ratio)).toAscii());
                    exit(1);
                }
                int moveOffsetY = subtitleProcessor->getMoveOffsetY();
                if (pos > 0)
                {
                    bool ok;
                    moveOffsetY = val.mid(pos + 1).toInt(&ok);
                    moveOffsetY = ok ? moveOffsetY : -1;
                    if (moveOffsetY < 0)
                    {
                        fprintf(stderr, (QString("ERROR: invalid pixel offset: %1")
                                         .arg(val.mid(pos + 1))).toAscii());
                        exit(1);
                    }
                    subtitleProcessor->setMoveOffsetY(moveOffsetY);
                }
                fprintf(stdout, (QString("OPTION: moving captions %1 %2:1 plus/minus %3 pixels")
                                 .arg(sm)
                                 .arg(QString::number(screenRatio, 'g', 6))
                                 .arg(QString::number(moveOffsetY))).toAscii());
            } break;
            case (int)Parameters::MOVE_X:
            {
                // move captions
                QString mx;
                pos = val.indexOf(',');
                if (pos > 0)
                {
                    mx = val.mid(0, pos);
                }
                else
                {
                    mx = val;
                }
                if (mx.toLower() == "left")
                {
                    subtitleProcessor->setMoveModeX(MoveModeX::LEFT);
                }
                else if (mx.toLower() == "right")
                {
                    subtitleProcessor->setMoveModeX(MoveModeX::RIGHT);
                }
                else if (mx.toLower() == "center")
                {
                    subtitleProcessor->setMoveModeX(MoveModeX::CENTER);
                }
                else
                {
                    fprintf(stderr, (QString("ERROR: invalid moveX command: %1").arg(mx)).toAscii());
                    exit(1);
                }

                int moveOffsetX = subtitleProcessor->getMoveOffsetX();
                if (pos > 0)
                {
                    bool ok;
                    moveOffsetX = val.mid(pos + 1).toInt(&ok);
                    moveOffsetX = ok ? moveOffsetX : -1;
                    if (moveOffsetX < 0)
                    {
                        fprintf(stderr, (QString("ERROR: invalid pixel offset: %1")
                                         .arg(val.mid(pos + 1))).toAscii());
                        exit(1);
                    }
                    subtitleProcessor->setMoveOffsetX(moveOffsetX);
                }
                fprintf(stdout, (QString("OPTION: moving captions to the %1 plus/minus %2 pixels")
                                 .arg(mx)
                                 .arg(QString::number(moveOffsetX))).toAscii());
            } break;
            case (int)Parameters::CROP_Y:
            {
                // add a delay
                int cropY;
                bool ok;
                cropY = val.trimmed().toInt(&ok);
                cropY = ok ? cropY : -1;
                if (cropY >= 0)
                {
                    subtitleProcessor->setCropOfsY(cropY);
                    fprintf(stdout, (QString("OPTION: set delay to %1")
                                     .arg(QString::number(cropY))).toAscii());
                }
                else
                {
                    fprintf(stderr, (QString("ERROR: invalid crop y value: %1")
                                     .arg(val.mid(0, pos))).toAscii());
                    exit(1);
                }
            } break;
            case (int)Parameters::PALETTE_MODE:
            {
                // select palette mode
                if (val.toLower() == "keep")
                {
                    subtitleProcessor->setPaletteMode(PaletteMode::KEEP_EXISTING);
                }
                else if (val.toLower() == "create")
                {
                    subtitleProcessor->setPaletteMode(PaletteMode::NEW);
                }
                else if (val.toLower() == "dither")
                {
                    subtitleProcessor->setPaletteMode(PaletteMode::CREATE_DITHERED);
                }
                else
                {
                    fprintf(stderr, (QString("ERROR: invalid palette mode: %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }
                fprintf(stdout, (QString("OPTION: set palette mode to %1")
                                 .arg(val.toLower())).toAscii());
            } break;
            case (int)Parameters::VERBATIM:
            {
                // select verbatim console output
                subtitleProcessor->setVerbatim(switchOn);
                fprintf(stdout, (QString("OPTION: enabled verbatim output mode: %1")
                                 .arg(strSwitchOn)).toAscii());
            } break;
            case (int)Parameters::FILTER:
            {
                // select scaling filter
                int idx = scalingFilters.indexOf(val);
                if (idx != -1)
                {
                    subtitleProcessor->setScalingFilter((ScalingFilters)idx);
                    fprintf(stdout, (QString("OPTION: set scaling filter to: %1").arg(scalingFilters[idx])).toAscii());
                }
                else
                {
                    fprintf(stderr, (QString("ERROR: invalid scaling filter: %1").arg(val)).toAscii());
                    exit(1);
                }
            } break;
            case (int)Parameters::TMERGE:
            {
                // set maximum difference for merging captions
                double t = 0;
                bool ok;
                t = val.trimmed().toDouble(&ok) * 90.0;
                if (!ok)
                {
                    fprintf(stderr, (QString("ERROR: Illegal value for maximum merge time: %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }
                int ti = (int)(t + 0.5);
                subtitleProcessor->setMergePTSdiff(ti);
                fprintf(stdout, (QString("OPTION: set maximum merge time to %1")
                                 .arg(QString::number(ti / 90.0, 'g', 6))).toAscii());
            } break;
            case (int)Parameters::SCALE:
            {
                // free x/y scaling factors
                pos = val.indexOf(',');
                if (pos > 0)
                {
                    bool ok;
                    double scaleX = val.mid(0, pos).toDouble(&ok);
                    scaleX = ok ? scaleX : -1;
                    if (scaleX < subtitleProcessor->minScale || scaleX > subtitleProcessor->maxScale)
                    {
                        fprintf(stderr, (QString("ERROR: invalid x scaling factor: %1")
                                         .arg(val.mid(0, pos))).toAscii());
                        exit(1);
                    }
                    double scaleY = val.mid(pos + 1).toDouble(&ok);
                    scaleY = ok ? scaleY : -1;
                    if (scaleY < subtitleProcessor->minScale || scaleY > subtitleProcessor->maxScale)
                    {
                        fprintf(stderr, (QString("ERROR: invalid y scaling factor: %1")
                                         .arg(val.mid(pos + 1))).toAscii());
                        exit(1);
                    }
                    subtitleProcessor->setFreeScale(scaleX, scaleY);
                    fprintf(stdout, (QString("OPTION: set free scaling factors to %1, %2")
                                     .arg(QString::number(scaleX, 'g', 6))
                                     .arg(QString::number(scaleY, 'g', 6))).toAscii());
                }
                else
                {
                    fprintf(stderr, (QString("ERROR: invalid scale command (missing comma): %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }

            } break;
            case (int)Parameters::ALPHA_CROP:
            {
                // alpha threshold for cropping and patching background color to black
                if (ival <0 || ival > 255)
                {
                    fprintf(stderr, (QString("ERROR: Illegal number range for alpha cropping threshold: %1")
                                     .arg(val)).toAscii());
                    exit(1);
                }
                else
                {
                    subtitleProcessor->setAlphaCrop(ival);
                }
                fprintf(stdout, (QString("OPTION: set alpha cropping threshold to %1")
                                 .arg(QString::number(ival))).toAscii());
            } break;
            case (int)Parameters::EXPORT_PAL:
            {
                // export target palette in PGCEdit text format
                subtitleProcessor->setWritePGCEditPal(switchOn);
                fprintf(stdout, (QString("OPTION: export target palette in PGCEDit text format: %1")
                                 .arg(strSwitchOn)).toAscii());
            } break;
            case (int)Parameters::FIX_ZERO_ALPHA:
            {
                // fix zero alpha frame palette for SUB/IDX and SUP/IFO
                subtitleProcessor->setFixZeroAlpha(switchOn);
                fprintf(stdout, (QString("OPTION: fix zero alpha frame palette for SUB/IDX and SUP/IFO: %1")
                                 .arg(strSwitchOn)).toAscii());
            } break; // useless
            case (int)Parameters::FORCE_ALL:
            {
                // clear/set forced flag for all captions
                subtitleProcessor->setForceAll(switchOn ? SetState::SET : SetState::CLEAR );
                fprintf(stdout, (QString("OPTION: set forced state of all captions to: %1")
                                 .arg(strSwitchOn)).toAscii());
            } break;
            default: //UNKNOWN:
            {
                fprintf(stderr, (QString("ERROR: Illegal argument: %1")
                                 .arg(args[i])).toAscii());
                exit(1);
            }
            }
        }

        // Step 3
        // open GUI if trg file name is missing
        if (trg.isEmpty())
        {
            loadPath = QFileInfo(src).absoluteFilePath();
            if (!QFileInfo(loadPath).exists())
            {
                fprintf(stderr, (QString("ERROR: File '%1' does not exist.").arg(QDir::toNativeSeparators(loadPath))).toAscii());
                exit(1);
            }
            fromCLI = true;
            return false;
        }

        subtitleProcessor->setOutputResolution(r);

        if (!subtitleProcessor->getKeepFps() && !defineFPStrg)
        {
            subtitleProcessor->setFPSTrg(subtitleProcessor->getDefaultFPS(r));
            fprintf(stdout, (QString("Target frame rate set to %1fps\n")
                             .arg(QString::number(subtitleProcessor->getFPSTrg(), 'g', 6))).toAscii());
        }

        // Step 4
        // handle wildcards
        QStringList srcFileNames;
        QStringList trgFileNames;
        QString path;
        // multiple files
        if (src.indexOf('*') != -1)
        {
            path = QFileInfo(src).absolutePath();
            if (path.isNull() || path.isEmpty())
            {
                path = "./";
            }
            QDir dir(QFileInfo(src).absolutePath());
            QStringList filters("*.sup");
            QFileInfoList srcFiles = dir.entryInfoList(filters, QDir::Files);

            if (srcFiles.size() == 0)
            {
                fprintf(stderr, (QString("ERROR: No match found for %1'")
                                 .arg(QString(path + "/" + src))).toAscii());
                exit(1);
            }
            if (trg.indexOf('*') == -1)
            {
                fprintf(stderr, QString("ERROR: No wildcards in target string!").toAscii());
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
            srcFileNames.insert(0, QFileInfo(src).absoluteFilePath());
            int aPos = trg.indexOf('*');
            if (aPos != -1)
            {
                // replace asterisk by path+filename of source without the extension
                trgFileNames.insert(0, QFileInfo(trg.replace("*", QFileInfo(src).completeBaseName())).absoluteFilePath());
            }
            else
            {
                trgFileNames.insert(0, QFileInfo(trg).absoluteFilePath());
            }
        }

        // Step 5
        // the main loop
        for (int fileNumber = 0; fileNumber < srcFileNames.size(); ++fileNumber)
        {
            subtitleProcessor->setCliMode(true);
            src = srcFileNames[fileNumber];
            trg = trgFileNames[fileNumber];
            // ok, let's start
            try
            {
                fprintf(stdout, (QString("\nConverting %1\n").arg(modes[(int)mode])).toAscii());
                // check input file
                QFileInfo srcFileInfo(src);
                if (!srcFileInfo.exists())
                {
                    fprintf(stderr, (QString("ERROR: File '%1' does not exist.").arg(QDir::toNativeSeparators(src))).toAscii());
                    exit(1);
                }
                bool xml = srcFileInfo.completeSuffix().toLower() == "xml";
                bool idx = srcFileInfo.completeSuffix().toLower() == "idx";
                bool ifo = srcFileInfo.completeSuffix().toLower() == "ifo";
                QByteArray id = subtitleProcessor->getFileID(src, 4);
                StreamID sid = (id.isEmpty()) ? StreamID::UNKNOWN : subtitleProcessor->getStreamID(id);
                if (!idx && !xml && !ifo && sid == StreamID::UNKNOWN)
                {
                    fprintf(stderr, (QString("File '%1' is not a supported subtitle stream.").arg(src)).toAscii());
                    exit(1);
                }
                // check output file(s)
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
                        fprintf(stderr, (QString("Target file '%1' is write protected.").arg(trg)).toAscii());
                        exit(1);
                    }
                }

                subtitleProcessor->setLoadPath(src);
                // read input file
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
                else
                {
                    if (QFileInfo(QString("%1/%2.ifo").arg(srcFileInfo.absolutePath()).arg(srcFileInfo.completeBaseName())).exists())
                    {
                        subtitleProcessor->readDVDSubStream(sid, false);
                    }
                    else
                    {
                        subtitleProcessor->readSup();
                    }
                }

                subtitleProcessor->scanSubtitles();
                printWarnings();
                // move captions
                if (subtitleProcessor->getMoveModeX() != MoveModeX::KEEP || subtitleProcessor->getMoveModeY() != MoveModeY::KEEP)
                {
                    subtitleProcessor->setCineBarFactor((1.0 - (16.0/9)/screenRatio)/2.0);
                    subtitleProcessor->moveAllToBounds();
                }
                // set some values
                if (subtitleProcessor->getExportForced() && subtitleProcessor->getNumForcedFrames()==0)
                {
                    fprintf(stderr, QString("No forced subtitles found.").toAscii());
                    exit(1);
                }
                QVector<int> lt = subtitleProcessor->getLuminanceThreshold();
                if (lumThr1 > 0)
                {
                    lt.replace(1, lumThr1);
                }
                if (lumThr2 > 0)
                {
                    lt.replace(0, lumThr2);
                }
                subtitleProcessor->setLuminanceThreshold(lt);
                subtitleProcessor->setAlphaThreshold(alphaThr);
                if (langIdx != -1)
                {
                    subtitleProcessor->setLanguageIdx(langIdx);
                }
                // write output
                subtitleProcessor->writeSub(trg);
            }
            catch(QString e)
            {
                fprintf(stderr, QString("ERROR: " + e).toAscii());
                exit(1);
            }
            // clean up
            printWarnings();
            subtitleProcessor->exit();
        }
        fprintf(stdout, (QString("\nConversion of %1 file(s) finished\n").arg(QString::number(srcFileNames.size()))).toAscii());
    }
    exit(0);
}

void BDSup2Sub::Redirect_console()
{
#ifdef Q_WS_WIN
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

    ui->subtitleImage->setImage(0, 1, 1);
    ui->subtitleImage->update();
    ui->sourceImage->setImage(0);
    ui->sourceImage->update();
    ui->targetImage->setImage(0);
    ui->targetImage->update();

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
    QStringList colorNames = { "white", "light gray", "dark gray",
                               "Color 1 light", "Color 1 dark",
                               "Color 2 light", "Color 2 dark",
                               "Color 3 light", "Color 3 dark",
                               "Color 4 light", "Color 4 dark",
                               "Color 5 light", "Color 5 dark",
                               "Color 6 light", "Color 6 dark"
                             };

    QVector<QColor> colors;
    QVector<QColor> defaultColors;

    for (int i = 0; i < 15; ++i)
    {
        colors.push_back(subtitleProcessor->getCurrentDVDPalette()->getColor(i + 1));
        defaultColors.push_back(subtitleProcessor->getDefaultDVDPalette()->getColor(i + 1));
    }

    colorDialog.setParameters(colorNames, colors, defaultColors);
    colorDialog.setPath(colorPath);
    if (colorDialog.exec() != QDialog::Rejected)
    {
        colors = colorDialog.getColors();
        colorPath = colorDialog.getPath();
        for (int i = 0; i < colors.size(); ++i)
        {
            subtitleProcessor->getCurrentDVDPalette()->setColor(i + 1, colors[i]);
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
        colors.push_back(subtitleProcessor->getCurrentSrcDVDPalette()->getColor(i));
        defaultColors.push_back(subtitleProcessor->getDefaultSrcDVDPalette()->getColor(i));
    }

    colorDialog.setParameters(colorNames, colors, defaultColors);
    colorDialog.setPath(colorPath);
    if (colorDialog.exec() != QDialog::Rejected)
    {
        colors = colorDialog.getColors();
        colorPath = colorDialog.getPath();
        Palette* palette = new Palette(colors.size(), true);
        for (int i = 0; i < colors.size(); ++i)
        {
            palette->setColor(i, colors[i]);
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
    FramePaletteDialog framePaletteDialog(this, subtitleProcessor);
    framePaletteDialog.setIndex(subIndex);
    framePaletteDialog.exec();
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

void BDSup2Sub::moveAllCaptions_triggered()
{
    MoveDialog moveDialog(this, subtitleProcessor);
    moveDialog.setIndex(subIndex);
    moveDialog.exec();
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

void BDSup2Sub::resetCropOffset_triggered()
{
    subtitleProcessor->setCropOfsY(0);
    ui->subtitleImage->setCropOfsY(0);
    ui->subtitleImage->update();
}

void BDSup2Sub::refreshSrcFrame(int index)
{
    QImage* image = subtitleProcessor->getSrcImage();
    ui->sourceImage->setImage(image);
    ui->sourceInfoLabel->setText(subtitleProcessor->getSrcInfoStr(index));
}

void BDSup2Sub::refreshTrgFrame(int index)
{
    ui->subtitleImage->setDimension(subtitleProcessor->getTrgWidth(index), subtitleProcessor->getTrgHeight(index));
    ui->subtitleImage->setOffsets(subtitleProcessor->getTrgOfsX(index), subtitleProcessor->getTrgOfsY(index));
    ui->subtitleImage->setCropOfsY(subtitleProcessor->getCropOfsY());
    ui->subtitleImage->setImage(subtitleProcessor->getTrgImage(), subtitleProcessor->getTrgImgWidth(index), subtitleProcessor->getTrgImgHeight(index));
    ui->subtitleImage->setExcluded(subtitleProcessor->getTrgExcluded(index));
    QImage *image = subtitleProcessor->getTrgImage();
    ui->targetImage->setImage(image);
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

void BDSup2Sub::on_subtitleNumberComboBox_editTextChanged(const QString &index)
{
    if (index.isEmpty() || index.isNull())
    {
        ui->subtitleNumberComboBox->setPalette(*errorBackground);
        return;
    }
    ui->subtitleNumberComboBox->setPalette(*okBackground);

    ui->subtitleNumberComboBox->blockSignals(true);
    subIndex = index.toInt() - 1;
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
    ui->subtitleNumberComboBox->blockSignals(false);
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

void BDSup2Sub::on_hiMedThresholdComboBox_editTextChanged(const QString &arg1)
{
    QVector<int> lumaThreshold = subtitleProcessor->getLuminanceThreshold();
    if (arg1.isEmpty() || arg1.isNull() || arg1.toInt() <= lumaThreshold[1])
    {
        ui->hiMedThresholdComboBox->setPalette(*errorBackground);
        return;
    }
    ui->hiMedThresholdComboBox->setPalette(*okBackground);
    ui->hiMedThresholdComboBox->blockSignals(true);
    lumaThreshold[0] = arg1.toInt();
    subtitleProcessor->setLuminanceThreshold(lumaThreshold);
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

void BDSup2Sub::on_medLowThresholdComboBox_editTextChanged(const QString &arg1)
{
    QVector<int> lumaThreshold = subtitleProcessor->getLuminanceThreshold();
    if (arg1.isEmpty() || arg1.isNull() || arg1.toInt() >= lumaThreshold[0])
    {
        ui->medLowThresholdComboBox->setPalette(*errorBackground);
        return;
    }
    ui->medLowThresholdComboBox->setPalette(*okBackground);
    ui->medLowThresholdComboBox->blockSignals(true);
    lumaThreshold[1] = arg1.toInt();
    subtitleProcessor->setLuminanceThreshold(lumaThreshold);
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

void BDSup2Sub::on_alphaThresholdComboBox_editTextChanged(const QString &arg1)
{
    if (arg1.isEmpty() || arg1.isNull())
    {
        ui->alphaThresholdComboBox->setPalette(*errorBackground);
        return;
    }
    ui->alphaThresholdComboBox->setPalette(*okBackground);
    ui->alphaThresholdComboBox->blockSignals(true);
    subtitleProcessor->setAlphaThreshold(arg1.toInt());
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
    ui->alphaThresholdComboBox->blockSignals(false);
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
