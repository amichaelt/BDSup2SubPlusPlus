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
#include <QPixmap>
#include "types.h"
#include "conversiondialog.h"
#include "progressdialog.h"
#include <QMessageBox>

BDSup2Sub::BDSup2Sub(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDSup2Sub)
{
    ui->setupUi(this);
    this->setWindowTitle(progNameVer);

    QFont font("Monospace", 9);
    font.setStyleHint(QFont::TypeWriter);
    ui->consoleOutput->setFont(font);

    ui->consoleOutput->insertPlainText("BDSup2Sub 4.0.1 - a converter from Blu-Ray/HD-DVD SUP to DVD SUB/IDX and more\n");
    ui->consoleOutput->insertPlainText("0xdeadbeef, mjuhasz 06-01-2012.\n");
    ui->consoleOutput->insertPlainText("Official thread at Doom9: http://forum.doom9.org/showthread.php?t=145277\n");

    fillComboBoxes();

    connect(ui->action_Load, SIGNAL(triggered()), this, SLOT(openFile()));

    subtitleProcessor = new SubtitleProcessor;
    connect(subtitleProcessor, SIGNAL(windowTitleChanged(QString)), this, SLOT(changeWindowTitle(QString)));
    progressDialog = new ProgressDialog(this);
    connect(subtitleProcessor, SIGNAL(progressDialogTextChanged(QString)), progressDialog, SLOT(setText(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogTitleChanged(QString)), progressDialog, SLOT(setWindowTitle(QString)));
    connect(subtitleProcessor, SIGNAL(progressDialogValueChanged(int)), progressDialog, SLOT(setCurrentValue(int)));
    connect(subtitleProcessor, SIGNAL(progressDialogVisibilityChanged(bool)), progressDialog, SLOT(setVisible(bool)));
    connect(subtitleProcessor, SIGNAL(loadingSubtitleFinished()), this, SLOT(onLoadingSubtitleFileFinished()));
    connect(subtitleProcessor, SIGNAL(moveAllFinished()), this, SLOT(convertSup()));
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
    for (int i = 0; i < num; ++i)
    {
        ui->subtitleNumberComboBox->addItem(QString::number(i));
    }
    ui->subtitleNumberComboBox->setCurrentIndex(subIndex);
    ui->alphaThresholdComboBox->setCurrentIndex(subtitleProcessor->getAlphaThreshold());
    ui->hiMedThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[0]);
    ui->medLowThresholdComboBox->setCurrentIndex(subtitleProcessor->getLuminanceThreshold()[1]);

    if (subtitleProcessor->getCropOfsY() > 0)
    {
        if (QMessageBox::question(this, "", "Reset Crop Offset?", QMessageBox::Yes, QMessageBox::No == QMessageBox::Yes))
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
        else
        {
            convertSup();
        }
    }
    else
    {
        closeSubtitle();
        //TODO: print warning about unsupported file
        subtitleProcessor->close();
    }
}

void BDSup2Sub::convertSup()
{
    subtitleProcessor->convertSup(subIndex, subIndex + 1, subtitleProcessor->getNumberOfFrames());
    refreshSrcFrame(subIndex);
    refreshTrgFrame(subIndex);

    enableCoreComponents(true);

    if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getInputMode() == InputMode::SUPIFO);
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
    loadPath = fileName;
    loadSubtitleFile();
}

void BDSup2Sub::fillComboBoxes()
{
    for (int i = 0; i < 256; ++i)
    {
        ui->alphaThresholdComboBox->addItem(QString::number(i));
        ui->medLowThresholdComboBox->addItem(QString::number(i));
        ui->hiMedThresholdComboBox->addItem(QString::number(i));
    }
    ui->alphaThresholdComboBox->setCurrentIndex(80);
    ui->medLowThresholdComboBox->setCurrentIndex(160);
    ui->hiMedThresholdComboBox->setCurrentIndex(210);
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
}

void BDSup2Sub::enableVobSubMenuCombo()
{
    bool enable;

    if ((subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getOutputMode() == OutputMode::SUPIFO)
            && ((subtitleProcessor->getInputMode() != InputMode::VOBSUB || subtitleProcessor->getInputMode() != InputMode::SUPIFO)
                || subtitleProcessor->getPaletteMode() != PaletteMode::KEEP_EXISTING))
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"),
                                                   loadPath.isEmpty() ? QDir::currentPath() : loadPath,
                                                    filter,
                                                    &selectedFilter
                                                    );

    if (fileName.isNull() || fileName.isEmpty() || !QFile(fileName).exists()) return;

    loadPath = fileName;

    loadSubtitleFile();
}

void BDSup2Sub::loadSubtitleFile()
{
    subIndex = 0;
    saveFileName = QFileInfo(loadPath).completeBaseName();
    savePath = QFileInfo(loadPath).absoluteDir().absolutePath();

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
    ui->subtitleNumberComboBox->clear();
    enableCoreComponents(false);
    updateRecentMenu();
    ui->paletteComboBox->setEnabled(false);
    ui->alphaThresholdComboBox->setEnabled(false);
    ui->hiMedThresholdComboBox->setEnabled(false);
    ui->medLowThresholdComboBox->setEnabled(false);

    //TODO: finish implementing
    throw 10;
}

void BDSup2Sub::updateRecentMenu()
{
    ui->menu_Recent_Files->setEnabled(false);
    if (subtitleProcessor->getRecentFiles().size() > 0)
    {
        ui->menu_Recent_Files->clear();
        for (auto recentFile : subtitleProcessor->getRecentFiles())
        {
            ui->menu_Recent_Files->addAction(QDir::toNativeSeparators(recentFile));
        }
        ui->menu_Recent_Files->setEnabled(true);
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
