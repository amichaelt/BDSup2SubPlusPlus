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

#include "conversiondialog.h"
#include "ui_conversiondialog.h"
#include "Subtitles/subtitleprocessor.h"
#include "Subtitles/subpicture.h"

ConversionDialog::ConversionDialog(QWidget *parent, SubtitleProcessor *subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::ConversionDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    this->subtitleProcessor = subtitleProcessor;

    changeResolution = subtitleProcessor->getConvertResolution();

    if (!changeResolution && subtitleProcessor->getNumberOfFrames() > 0)
    {
        resolution = subtitleProcessor->getResolution(subtitleProcessor->getSubPictureSrc(0)->width, subtitleProcessor->getSubPictureSrc(0)->height);
    }
    else
    {
        resolution = subtitleProcessor->getOutputResolution();
    }

    moveCaptions = subtitleProcessor->getMoveCaptions();
    ui->applyMoveAllSettingsCheckBox->setEnabled(false);
    ui->applyMoveAllSettingsCheckBox->setChecked(moveCaptions);

    delayPTS = subtitleProcessor->getDelayPTS();
    minTimePTS = (int)subtitleProcessor->syncTimePTS(subtitleProcessor->getMinTimePTS(), subtitleProcessor->getFPSTrg());
    changeFPS = subtitleProcessor->getConvertFPS();
    changeScale = subtitleProcessor->getApplyFreeScale();
    fixShortFrames = subtitleProcessor->getFixShortFrames();
    fpsSrc = subtitleProcessor->getFPSSrc();
    fpsTrg = subtitleProcessor->getFPSTrg();
    scaleX = subtitleProcessor->getFreeScaleX();
    scaleY = subtitleProcessor->getFreeScaleY();
    fpsSrcCertain = subtitleProcessor->getFpsSrcCertain();
    cancel = false;

    ui->resolutionComboBox->setCurrentIndex((int)resolution);
    ui->resolutionComboBox->setEnabled(changeResolution);
    ui->convertResolutionComboBox->setChecked(changeResolution);

    ui->delayLineEdit->setText(QString::number(delayPTS / 90.0, 'g', 5));
    ui->changeFrameRateCheckBox->setChecked(changeFPS);
    ui->sourceFramerateComboBox->setCurrentIndex(ui->sourceFramerateComboBox->findText(QString::number(fpsSrc)));
    ui->sourceFramerateComboBox->setEnabled(changeFPS);
    ui->targetFramerateComboBox->setCurrentIndex(ui->sourceFramerateComboBox->findText(QString::number(fpsTrg)));
    ui->targetFramerateComboBox->setEnabled(true);
    ui->minTimeLineEdit->setText(QString::number(minTimePTS / 90.0, 'g', 5));
    ui->minTimeLineEdit->setEnabled(fixShortFrames);
    ui->fixTooShortFramesCheckBox->setEnabled(true);
    ui->fixTooShortFramesCheckBox->setChecked(fixShortFrames);
    ui->scaleCheckBox->setChecked(changeScale);
    ui->scaleXLineEdit->setText(QString::number(scaleX, 'g', 5));
    ui->scaleXLineEdit->setEnabled(changeScale);
    ui->scaleYLineEdit->setText(QString::number(scaleY, 'g', 5));
    ui->scaleYLineEdit->setEnabled(changeScale);

    ui->forceFlagsComboBox->setCurrentIndex((int) forcedState);

    //QPalette pal = ui->delayLineEdit->palette();
    //pal.setColor(ui->delayLineEdit->backgroundRole(), Qt::blue);
    //ui->delayLineEdit->setPalette(pal);
}

ConversionDialog::~ConversionDialog()
{
    delete ui;
}

void ConversionDialog::enableOptionMove(bool enable)
{
    ui->applyMoveAllSettingsCheckBox->setEnabled(enable);
}

void ConversionDialog::on_cancelButton_clicked()
{
    reject();
}

void ConversionDialog::on_okButton_clicked()
{
    accept();
}
