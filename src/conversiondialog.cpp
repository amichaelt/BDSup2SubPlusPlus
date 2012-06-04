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

#include <QDoubleValidator>
#include <QRegExpValidator>

ConversionDialog::ConversionDialog(QWidget *parent, SubtitleProcessor *subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::ConversionDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    this->subtitleProcessor = subtitleProcessor;

    QRegExp regex("[0-9]+([.][0-9]+)?|pal|25p|ntsc|30p|24p|50i|60i");
    fpsSrcValidator = new QRegExpValidator(regex);
    fpsTrgValidator = new QRegExpValidator(regex);
    scaleXValidator = new QDoubleValidator(subtitleProcessor->minScale, subtitleProcessor->maxScale, 9);
    scaleYValidator = new QDoubleValidator(subtitleProcessor->minScale, subtitleProcessor->maxScale, 9);
    delayPTSValidator = new QDoubleValidator();
    minTimePTSValidator = new QDoubleValidator();

    ui->sourceFramerateComboBox->setValidator(fpsSrcValidator);
    ui->sourceFramerateComboBox->setCompleter(0);
    ui->targetFramerateComboBox->setValidator(fpsTrgValidator);
    ui->targetFramerateComboBox->setCompleter(0);
    ui->scaleXLineEdit->setValidator(scaleXValidator);
    ui->scaleYLineEdit->setValidator(scaleYValidator);
    ui->delayLineEdit->setValidator(delayPTSValidator);
    ui->minTimeLineEdit->setValidator(minTimePTSValidator);

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
    forcedState = subtitleProcessor->getForceAll();
    fillDialog();
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
    subtitleProcessor->setConvertFPS(changeFPS);
    if (changeFPS)
    {
        subtitleProcessor->setFPSSrc(fpsSrc);
    }
    subtitleProcessor->setFPSTrg(fpsTrg);
    subtitleProcessor->setDelayPTS(delayPTS);
    subtitleProcessor->setFixShortFrames(fixShortFrames);
    subtitleProcessor->setMinTimePTS(minTimePTS);
    subtitleProcessor->setConvertResolution(changeResolution);
    if (changeResolution)
    {
        subtitleProcessor->setOutputResolution(resolution);
    }
    subtitleProcessor->setApplyFreeScale(changeScale);
    if (changeScale)
    {
        subtitleProcessor->setFreeScale(scaleX, scaleY);
    }
    subtitleProcessor->setForceAll(forcedState);
    if (ui->applyMoveAllSettingsCheckBox->isEnabled())
    {
        subtitleProcessor->setMoveCaptions(moveCaptions);
    }
    accept();
}

void ConversionDialog::on_convertResolutionCheckBox_toggled(bool checked)
{
    ui->resolutionComboBox->setEnabled(checked);
    changeResolution = checked;
}

void ConversionDialog::on_scaleCheckBox_toggled(bool checked)
{
    ui->scaleXLineEdit->setEnabled(checked);
    ui->scaleYLineEdit->setEnabled(checked);
    changeScale = checked;
}

void ConversionDialog::on_fixTooShortFramesCheckBox_toggled(bool checked)
{
    ui->minTimeLineEdit->setEnabled(checked);
    fixShortFrames = checked;
}

void ConversionDialog::on_changeFrameRateCheckBox_toggled(bool checked)
{
    ui->sourceFramerateComboBox->setEnabled(checked);
    changeFPS = checked;
}

void ConversionDialog::on_applyMoveAllSettingsCheckBox_toggled(bool checked)
{
    moveCaptions = checked;
}

void ConversionDialog::on_resetButton_clicked()
{
    fpsTrg = fpsSrc;
    delayPTS = subtitleProcessor->getDelayPTSDefault();
    fixShortFrames = subtitleProcessor->getFixShortFramesDefault();
    minTimePTS = subtitleProcessor->getMinTimePTSDefault();
    changeScale = subtitleProcessor->getApplyFreeScaleDefault();
    forcedState = SetState::KEEP;
    fillDialog();
}

void ConversionDialog::fillDialog()
{
    ui->resolutionComboBox->setCurrentIndex((int)resolution);
    ui->resolutionComboBox->setEnabled(changeResolution);
    ui->convertResolutionCheckBox->setChecked(changeResolution);

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
}

void ConversionDialog::on_resolutionComboBox_currentIndexChanged(int index)
{
    resolution = (Resolution)index;
    if (!subtitleProcessor->getKeepFps())
    {
        fpsTrg = subtitleProcessor->getDefaultFPS(resolution);
        ui->targetFramerateComboBox->setCurrentIndex(ui->targetFramerateComboBox->findText(QString::number(fpsTrg)));
    }
}

void ConversionDialog::on_sourceFramerateComboBox_editTextChanged(const QString &arg1)
{
    QString temp(arg1);
    int pos = 0;
    if (ui->sourceFramerateComboBox->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        fpsSrc = arg1.toDouble();
    }
}

void ConversionDialog::on_targetFramerateComboBox_editTextChanged(const QString &arg1)
{
    QString temp(arg1);
    int pos = 0;
    if (ui->targetFramerateComboBox->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        fpsTrg = subtitleProcessor->getFPS(arg1);
    }
}

void ConversionDialog::on_scaleXLineEdit_textChanged(const QString &arg1)
{
    QString temp(arg1);
    int pos = 0;
    if (ui->scaleXLineEdit->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        scaleX = subtitleProcessor->getFPS(arg1);
    }
}

void ConversionDialog::on_scaleYLineEdit_textChanged(const QString &arg1)
{
    QString temp(arg1);
    int pos = 0;
    if (ui->scaleYLineEdit->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        scaleY = arg1.toDouble();
    }
}

void ConversionDialog::on_delayLineEdit_textChanged(const QString &arg1)
{
    QString temp(arg1);
    int pos = 0;
    if (ui->delayLineEdit->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        delayPTS = (int)subtitleProcessor->syncTimePTS((long)(arg1.toDouble() * 90), fpsTrg);
    }
}

void ConversionDialog::on_minTimeLineEdit_textChanged(const QString &arg1)
{
    QString temp(arg1);
    int pos = 0;
    if (ui->minTimeLineEdit->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        minTimePTS = (int)subtitleProcessor->syncTimePTS((long)(arg1.toDouble() * 90), fpsTrg);
    }
}
