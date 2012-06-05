/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
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

#include <QMessageBox>
#include <QPixmap>

BDSup2Sub::BDSup2Sub(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDSup2Sub)
{
    ui->setupUi(this);

    okBackground = new QPalette(ui->subtitleNumberComboBox->palette());
    errorBackground = new QPalette();
    errorBackground->setColor(QPalette::Base, QColor(0xffe1acac));

    this->setWindowTitle(progNameVer);

    QFont font("Monospace", 9);
    font.setStyleHint(QFont::TypeWriter);
    ui->consoleOutput->setFont(font);

    ui->consoleOutput->insertPlainText(progNameVer + " - a converter from Blu-Ray/HD-DVD SUP to DVD SUB/IDX and more\n");
    ui->consoleOutput->insertPlainText(authorDate + "\n");
    ui->consoleOutput->insertPlainText("Official thread at Doom9: http://forum.doom9.org/showthread.php?t=145277\n");

    fillComboBoxes();

    connect(ui->action_Load, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->action_Save_Export, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->action_Close, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(ui->action_Conversion_Settings, SIGNAL(triggered()), this, SLOT(openConversionSettings()));
    connect(ui->subtitleImage, SIGNAL(onMouseClicked(QMouseEvent*)), this, SLOT(onEditPaneClicked(QMouseEvent*)));

    subtitleProcessor = new SubtitleProcessor;
    progressDialog = new ProgressDialog(this);
    connectSubtitleProcessor();
}

BDSup2Sub::~BDSup2Sub()
{
    delete ui;
}

void BDSup2Sub::changeWindowTitle(QString newTitle)
{
    this->setWindowTitle(newTitle);
}

void BDSup2Sub::onLoadingSubtitleFileFinished()
{
    //TODO: warning dialog

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

    conversionDialog = new ConversionDialog(this, subtitleProcessor);
    conversionDialog->enableOptionMove(subtitleProcessor->getMoveCaptions());
    if (conversionDialog->exec() != QDialog::Rejected)
    {
        subtitleProcessor->scanSubtitles();
        if (subtitleProcessor->getMoveCaptions())
        {
            QThread *workerThread = new QThread;
            subtitleProcessor->moveToThread(workerThread);
            connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(moveAll()));
            connect(subtitleProcessor, SIGNAL(moveAllFinished()), workerThread, SLOT(quit()));
            connect(subtitleProcessor, SIGNAL(moveAllFinished()), workerThread, SLOT(deleteLater()));

            workerThread->start();
        }
        convertSup();
    }
    else
    {
        closeSubtitle();
        //TODO: print warning about unsupported file
        subtitleProcessor->close();
    }
    delete conversionDialog;
}

void BDSup2Sub::convertSup()
{
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    refreshSrcFrame(subIndex);
    refreshTrgFrame(subIndex);

    enableCoreComponents(true);

    if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getInputMode() == InputMode::SUPIFO)
    {
        enableVobSubComponents(true);
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

void BDSup2Sub::connectSubtitleProcessor()
{
    if (subtitleProcessor != 0)
    {
        subtitleProcessor->close();
        subtitleProcessor->moveToThread(QApplication::instance()->thread());
        subtitleProcessor->disconnect();
    }
    connect(subtitleProcessor, SIGNAL(windowTitleChanged(QString)), this, SLOT(changeWindowTitle(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogTextChanged(QString)), progressDialog, SLOT(setText(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogTitleChanged(QString)), progressDialog, SLOT(setWindowTitle(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogValueChanged(int)), progressDialog, SLOT(setCurrentValue(int)));
    connect(subtitleProcessor, SIGNAL(progressDialogVisibilityChanged(bool)), progressDialog, SLOT(setVisible(bool)));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished()), this, SLOT(onLoadingSubtitleFileFinished()));
    connect(subtitleProcessor, SIGNAL(moveAllFinished()), this, SLOT(convertSup()));
}

void BDSup2Sub::fillComboBoxes()
{
    ui->alphaThresholdComboBox->blockSignals(true);
    ui->medLowThresholdComboBox->blockSignals(true);
    ui->hiMedThresholdComboBox->blockSignals(true);
    for (int i = 0; i < 256; ++i)
    {
        ui->alphaThresholdComboBox->addItem(QString::number(i));
        ui->medLowThresholdComboBox->addItem(QString::number(i));
        ui->hiMedThresholdComboBox->addItem(QString::number(i));
    }
    ui->alphaThresholdComboBox->setCurrentIndex(80);
    ui->medLowThresholdComboBox->setCurrentIndex(160);
    ui->hiMedThresholdComboBox->setCurrentIndex(210);

    ui->alphaThresholdComboBox->setValidator(alphaThresholdValidator);
    ui->medLowThresholdComboBox->setValidator(medLowThresholdValidator);
    ui->hiMedThresholdComboBox->setValidator(hiMedThresholdValidator);
    ui->alphaThresholdComboBox->blockSignals(false);
    ui->medLowThresholdComboBox->blockSignals(false);
    ui->hiMedThresholdComboBox->blockSignals(false);
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
        ui->paletteComboBox->setCurrentIndex((int)PaletteMode::CREATE_NEW);
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
    bool showException = true;
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
    exportDialog = new ExportDialog(this, path, subtitleProcessor);
    if (exportDialog->exec() != QDialog::Rejected)
    {
        QString fileName = exportDialog->getFileName();
        delete exportDialog;
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
                //TODO: error handling
                throw 10;
            }
            if (QMessageBox::question(this, "", "Target exists! Overwrite?", QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            {
                //TODO: error handling
                throw 10;
            }
        }
        connectSubtitleProcessor();
        QThread *workerThread = new QThread;
        subtitleProcessor->setLoadPath(fileName);
        subtitleProcessor->moveToThread(workerThread);
        connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(createSubtitleStream()));
        connect(subtitleProcessor, SIGNAL(writingSubtitleFinished()), workerThread, SLOT(quit()));
        connect(subtitleProcessor, SIGNAL(writingSubtitleFinished()), workerThread, SLOT(deleteLater()));
        workerThread->start();
    }
}

void BDSup2Sub::closeFile()
{
    closeSubtitle();
}

void BDSup2Sub::loadSubtitleFile()
{
    subIndex = 0;
    saveFileName = QFileInfo(loadPath).completeBaseName();
    savePath = QFileInfo(loadPath).absolutePath();

    enableCoreComponents(false);
    enableVobSubComponents(false);

    QThread *workerThread = new QThread;
    subtitleProcessor->setLoadPath(loadPath);
    subtitleProcessor->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), subtitleProcessor, SLOT(readSubtitleStream()));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished()), workerThread, SLOT(quit()));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished()), workerThread, SLOT(deleteLater()));

    workerThread->start();
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
    loadPath = QFileInfo(action->text()).absoluteFilePath();
    closeFile();
    connectSubtitleProcessor();
    loadSubtitleFile();
}

void BDSup2Sub::onEditPaneClicked(QMouseEvent *event)
{
    if (subtitleProcessor->getNumberOfFrames() > 0 && event->button() == Qt::LeftButton)
    {
        editDialog = new EditDialog(this, subtitleProcessor);
        editDialog->setIndex(subIndex);
        editDialog->exec();
        subIndex = editDialog->getIndex();
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
        refreshSrcFrame(subIndex);
        refreshTrgFrame(subIndex);
        ui->subtitleNumberComboBox->setCurrentIndex(subIndex);
        delete editDialog;
    }
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
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
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
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    refreshSrcFrame(subIndex);
    refreshTrgFrame(subIndex);
    ui->subtitleNumberComboBox->blockSignals(false);
}

void BDSup2Sub::on_outputFormatComboBox_currentIndexChanged(const QString &format)
{
    if (format.contains("IDX"))
    {
        subtitleProcessor->setOutputMode(OutputMode::VOBSUB);
    }
    else if (format.contains("IFO"))
    {
        subtitleProcessor->setOutputMode(OutputMode::SUPIFO);
    }
    else if (format.contains("SUP"))
    {
        subtitleProcessor->setOutputMode(OutputMode::BDSUP);
    }
    else
    {
        subtitleProcessor->setOutputMode(OutputMode::XML);
    }
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

    conversionDialog = new ConversionDialog(this, subtitleProcessor);
    conversionDialog->enableOptionMove(subtitleProcessor->getMoveCaptions());
    if (conversionDialog->exec() != QDialog::Rejected)
    {
        subtitleProcessor->reScanSubtitles(oldResolution, fpsTrgOld, delayOld, changeFpsOld, fsXOld, fsYOld);
        subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
        refreshTrgFrame(subIndex);
    }
    delete conversionDialog;
}

void BDSup2Sub::on_outputFormatComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setOutputMode((OutputMode)index);
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
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

void BDSup2Sub::on_paletteComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setPaletteMode((PaletteMode)index);
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    refreshTrgFrame(subIndex);
}

void BDSup2Sub::on_filterComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setScalingFilter((ScalingFilters)index);
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
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
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
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
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
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
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
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
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    refreshTrgFrame(subIndex);
    ui->medLowThresholdComboBox->blockSignals(false);
}

void BDSup2Sub::on_alphaThresholdComboBox_currentIndexChanged(int index)
{
    subtitleProcessor->setAlphaThreshold(index);
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
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
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    refreshTrgFrame(subIndex);
    ui->alphaThresholdComboBox->blockSignals(false);
}
