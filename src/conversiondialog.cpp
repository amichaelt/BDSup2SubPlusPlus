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

#include "conversiondialog.h"
#include "ui_conversiondialog.h"
#include "Subtitles/subtitleprocessor.h"
#include "Subtitles/subpicture.h"
#include "Tools/timeutil.h"
#include "types.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMessageBox>
#else
#include <QMessageBox>
#endif

#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QPalette>
#include <QKeyEvent>
#include <QSettings>

ConversionDialog::ConversionDialog(QWidget *parent, SubtitleProcessor *subtitleProcessor, QSettings* settings) :
    QDialog(parent),
    ui(new Ui::ConversionDialog),
    forcedState(SetState::KEEP)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    this->subtitleProcessor = subtitleProcessor;
    this->settings = settings;

    QRegExp regex("[0-9]+([.][0-9]+)?|pal|25p|ntsc|30p|24p|50i|60i");
    fpsSrcValidator = new QRegExpValidator(regex);
    fpsTrgValidator = new QRegExpValidator(regex);
    scaleXValidator = new QDoubleValidator;
    scaleYValidator = new QDoubleValidator;
    delayPTSValidator = new QDoubleValidator;
    minTimePTSValidator = new QDoubleValidator;

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
        resolution = subtitleProcessor->getResolution(subtitleProcessor->getSubPictureSrc(0)->screenWidth(), subtitleProcessor->getSubPictureSrc(0)->screenHeight());
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
    if (changeFPS)
    {
        fpsTrg = subtitleProcessor->getFPSTrg();
    }
    else
    {
        fpsTrg = fpsSrc;
    }
    scaleX = subtitleProcessor->getFreeScaleX();
    scaleY = subtitleProcessor->getFreeScaleY();
    fpsSrcCertain = subtitleProcessor->getFpsSrcCertain();
    forcedState = subtitleProcessor->getForceAll();

    okBackground = new QPalette(ui->scaleXLineEdit->palette());
    errorBackground = new QPalette();
    errorBackground->setColor(QPalette::Base, QColor(0xffe1acac));
    warnBackground = new QPalette();
    warnBackground->setColor(QPalette::Base, QColor(0xffffffc0));

    fillDialog();
}

ConversionDialog::~ConversionDialog()
{
    delete fpsSrcValidator;
    delete fpsTrgValidator;
    delete scaleXValidator;
    delete scaleYValidator;
    delete delayPTSValidator;
    delete minTimePTSValidator;
    delete okBackground;
    delete errorBackground;
    delete warnBackground;
    delete ui;
}

void ConversionDialog::enableOptionMove(bool enable)
{
    ui->applyMoveAllSettingsCheckBox->setEnabled(enable);
}

void ConversionDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if (ui->delayLineEdit->hasFocus())
        {
            isReady = true;
            on_delayLineEdit_editingFinished();
            isReady = false;
            return;
        }
        if (ui->minTimeLineEdit->hasFocus())
        {
            isReady = true;
            on_minTimeLineEdit_editingFinished();
            isReady = false;
            return;
        }
        if (ui->scaleXLineEdit->hasFocus())
        {
            isReady = true;
            on_scaleXLineEdit_editingFinished();
            isReady = false;
            return;
        }
        if (ui->scaleYLineEdit->hasFocus())
        {
            isReady = true;
            on_scaleYLineEdit_editingFinished();
            isReady = false;
            return;
        }
        if (ui->sourceFramerateComboBox->hasFocus())
        {
            isReady = true;
            on_sourceFramerateComboBox_editingFinished();
            isReady = false;
            return;
        }
        if (ui->targetFramerateComboBox->hasFocus())
        {
            isReady = true;
            on_targetFramerateComboBox_editingFinished();
            isReady = false;
            return;
        }
    }
    QDialog::keyPressEvent(event);
}

void ConversionDialog::on_cancelButton_clicked()
{
    reject();
}

void ConversionDialog::on_okButton_clicked()
{
    if (subtitleProcessor->getNumberOfFrames() > 0)
    {
        if ((subtitleProcessor->getSubPictureSrc(0)->startTime() + delayPTS) < 0)
        {
            QMessageBox::warning(this, "Warning!", QString("First subpicture has timestamp of %1.\n"
                                 "Delay value of %2 is insufficient to correct this.")
                                 .arg(TimeUtil::ptsToTimeStr(subtitleProcessor->getSubPictureSrc(0)->startTime()))
                                 .arg(QString::number(delayPTS)));
            return;
        }

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
    ui->resolutionComboBox->blockSignals(true);
    ui->convertResolutionCheckBox->blockSignals(true);
    ui->delayLineEdit->blockSignals(true);
    ui->changeFrameRateCheckBox->blockSignals(true);
    ui->sourceFramerateComboBox->blockSignals(true);
    ui->targetFramerateComboBox->blockSignals(true);
    ui->minTimeLineEdit->blockSignals(true);
    ui->fixTooShortFramesCheckBox->blockSignals(true);
    ui->scaleCheckBox->blockSignals(true);
    ui->scaleXLineEdit->blockSignals(true);
    ui->scaleYLineEdit->blockSignals(true);
    ui->forcedFlagsGroupBox->blockSignals(true);

    ui->resolutionComboBox->setCurrentIndex((int)resolution);
    ui->resolutionComboBox->setEnabled(changeResolution);
    ui->convertResolutionCheckBox->setChecked(changeResolution);

    ui->delayLineEdit->setText(QString::number(delayPTS / 90.0, 'g', 6));
    ui->changeFrameRateCheckBox->setChecked(changeFPS);
    QString srcFrameRate = QString::number(fpsSrc, 'g', 5);
    ui->sourceFramerateComboBox->setCurrentIndex(ui->sourceFramerateComboBox->findText(srcFrameRate));
    ui->sourceFramerateComboBox->setEnabled(changeFPS);
    QString trgFrameRate = QString::number(fpsTrg, 'g', 5);
    ui->targetFramerateComboBox->setCurrentIndex(ui->sourceFramerateComboBox->findText(trgFrameRate));
    ui->targetFramerateComboBox->setEnabled(true);
    ui->minTimeLineEdit->setText(QString::number(minTimePTS / 90.0, 'g', 6));
    ui->minTimeLineEdit->setEnabled(fixShortFrames);
    ui->fixTooShortFramesCheckBox->setEnabled(true);
    ui->fixTooShortFramesCheckBox->setChecked(fixShortFrames);
    ui->scaleCheckBox->setChecked(changeScale);
    ui->scaleXLineEdit->setText(QString::number(scaleX, 'g', 6));
    ui->scaleXLineEdit->setEnabled(changeScale);
    ui->scaleYLineEdit->setText(QString::number(scaleY, 'g', 6));
    ui->scaleYLineEdit->setEnabled(changeScale);

    ui->forceFlagsComboBox->setCurrentIndex((int) forcedState);

    ui->resolutionComboBox->blockSignals(false);
    ui->convertResolutionCheckBox->blockSignals(false);
    ui->delayLineEdit->blockSignals(false);
    ui->changeFrameRateCheckBox->blockSignals(false);
    ui->sourceFramerateComboBox->blockSignals(false);
    ui->targetFramerateComboBox->blockSignals(false);
    ui->minTimeLineEdit->blockSignals(false);
    ui->fixTooShortFramesCheckBox->blockSignals(false);
    ui->scaleCheckBox->blockSignals(false);
    ui->scaleXLineEdit->blockSignals(false);
    ui->scaleYLineEdit->blockSignals(false);
    ui->forcedFlagsGroupBox->blockSignals(false);
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

void ConversionDialog::on_scaleXLineEdit_textChanged(const QString &arg1)
{
    if (arg1.isNull() || arg1.isEmpty())
    {
        ui->scaleXLineEdit->setPalette(*errorBackground);
        return;
    }
    double d = arg1.toDouble();
    if (d >= subtitleProcessor->minScale && d <= subtitleProcessor->maxScale)
    {
        scaleX = d;
        ui->scaleXLineEdit->setPalette(*okBackground);
    }
    else
    {
        ui->scaleXLineEdit->setPalette(*errorBackground);
    }
}

void ConversionDialog::on_scaleYLineEdit_textChanged(const QString &arg1)
{
    if (arg1.isNull() || arg1.isEmpty())
    {
        ui->scaleYLineEdit->setPalette(*errorBackground);
        return;
    }
    double d = arg1.toDouble();
    if (d >= subtitleProcessor->minScale && d <= subtitleProcessor->maxScale)
    {
        scaleY = d;
        ui->scaleYLineEdit->setPalette(*okBackground);
    }
    else
    {
        ui->scaleYLineEdit->setPalette(*errorBackground);
    }
}

void ConversionDialog::on_delayLineEdit_textChanged(const QString &arg1)
{
    if (arg1.isNull() || arg1.isEmpty())
    {
        ui->delayLineEdit->setPalette(*errorBackground);
        return;
    }
    QString temp(arg1);
    int pos = 0;
    if (ui->delayLineEdit->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        delayPTS = (int)subtitleProcessor->syncTimePTS((qint64)(arg1.toDouble() * 90), fpsTrg);
        if (temp != QString::number(delayPTS / 90.0, 'g', 6))
        {
            ui->delayLineEdit->setPalette(*warnBackground);
        }
        else
        {
            ui->delayLineEdit->setPalette(*okBackground);
        }
    }
}

void ConversionDialog::on_minTimeLineEdit_textChanged(const QString &arg1)
{
    if (arg1.isNull() || arg1.isEmpty())
    {
        ui->minTimeLineEdit->setPalette(*errorBackground);
        return;
    }
    QString temp(arg1);
    int pos = 0;
    if (ui->minTimeLineEdit->validator()->validate(temp, pos) == QValidator::Acceptable)
    {
        minTimePTS = (int)subtitleProcessor->syncTimePTS((qint64)(arg1.toDouble() * 90), fpsTrg);
        if (temp != QString::number(delayPTS / 90.0, 'g', 6))
        {
            ui->minTimeLineEdit->setPalette(*warnBackground);
        }
        else
        {
            ui->minTimeLineEdit->setPalette(*okBackground);
        }
    }
}

void ConversionDialog::on_forceFlagsComboBox_currentIndexChanged(int index)
{
    forcedState = (SetState)index;
}

void ConversionDialog::on_storeButton_clicked()
{
    QString s;
    double d;

    // fps source
    settings->setValue("convertFPS", QVariant(changeFPS));
    if (changeFPS)
    {
        s = ui->sourceFramerateComboBox->currentText();
        d = subtitleProcessor->getFPS(s);
        if (d > 0)
        {
            fpsSrc = d;
            settings->setValue("fpsSrc", QVariant(fpsSrc));
        }
    }

    // fps target
    s = ui->targetFramerateComboBox->currentText();
    d = subtitleProcessor->getFPS(s);
    if (d > 0)
    {
        fpsTrg = d;
        settings->setValue("fpsTrg", QVariant(fpsTrg));
    }

    // delay
    bool ok;
    d = ui->delayLineEdit->text().toDouble(&ok);
    if (ok)
    {
        delayPTS = (int)subtitleProcessor->syncTimePTS((qint64)(d * 90.0), fpsTrg);
        settings->setValue("delayPTS", QVariant(delayPTS));
    }

    // min time
    settings->setValue("fixShortFrames", QVariant(fixShortFrames));
    d = ui->minTimeLineEdit->text().toDouble(&ok);
    if (ok)
    {
        minTimePTS = (int)subtitleProcessor->syncTimePTS((qint64)d * 90.0, fpsTrg);
        settings->setValue("minTimePTS", minTimePTS);
    }

    // exit
    settings->setValue("changeResolution", QVariant(changeResolution));
    if (changeResolution)
    {
        settings->setValue("resolution", QVariant(subtitleProcessor->getResolutionName(resolution)));
    }

    // scaleX
    d = ui->scaleXLineEdit->text().toDouble(&ok);
    d = ok ? d : -1;
    if (d > 0)
    {
        if (d > subtitleProcessor->maxScale)
        {
            d = subtitleProcessor->maxScale;
        }
        else if (d < subtitleProcessor->minScale)
        {
            d = subtitleProcessor->minScale;
        }
        scaleX = d;
    }

    // scaleY
    d = ui->scaleYLineEdit->text().toDouble(&ok);
    d = ok ? d : -1;
    if (d > 0)
    {
        if (d > subtitleProcessor->maxScale)
        {
            d = subtitleProcessor->maxScale;
        }
        else if (d < subtitleProcessor->minScale)
        {
            d = subtitleProcessor->minScale;
        }
        scaleY = d;
    }

    // set scale X/Y
    settings->setValue("applyFreeScale", QVariant(changeScale));
    if (changeScale)
    {
        subtitleProcessor->storeFreeScale(scaleX, scaleY);
    }

    subtitleProcessor->storeSettings();
}

void ConversionDialog::on_restoreButton_clicked()
{
    changeResolution = subtitleProcessor->restoreConvertResolution();
    if (changeResolution)
    {
        resolution = subtitleProcessor->restoreResolution();
    }
    changeFPS = subtitleProcessor->restoreConvertFPS();
    if (changeFPS && !fpsSrcCertain)
    {
        fpsSrc = subtitleProcessor->restoreFpsSrc();
    }
    fpsTrg = subtitleProcessor->restoreFpsTrg();
    delayPTS = subtitleProcessor->restoreDelayPTS();
    fixShortFrames = subtitleProcessor->restoreFixShortFrames();
    minTimePTS = subtitleProcessor->restoreMinTimePTS();
    changeScale = subtitleProcessor->restoreApplyFreeScale();
    if (changeScale)
    {
        scaleX = subtitleProcessor->restoreFreeScaleX();
        scaleY = subtitleProcessor->restoreFreeScaleY();
    }
    forcedState = subtitleProcessor->getForceAll();
    fillDialog();
}

void ConversionDialog::on_scaleXLineEdit_editingFinished()
{
    if (!isReady) return;
    QString text = ui->scaleXLineEdit->text();
    int pos = 0;
    if (ui->scaleXLineEdit->validator()->validate(text, pos) == QValidator::Acceptable)
    {
        double d = text.toDouble();
        if (d < 0.5f)
        {
            d = 0.5;
        }
        else if (d > 2.0f)
        {
            d = 2.0;
        }
        scaleX = d;
    }
    ui->scaleXLineEdit->setText(QString::number(scaleX));
    ui->scaleXLineEdit->setPalette(*okBackground);
}

void ConversionDialog::on_scaleYLineEdit_editingFinished()
{
    if (!isReady) return;
    QString text = ui->scaleYLineEdit->text();
    int pos = 0;
    if (ui->scaleYLineEdit->validator()->validate(text, pos) == QValidator::Acceptable)
    {
        double d = text.toDouble();
        if (d < 0.5f)
        {
            d = 0.5;
        }
        else if (d > 2.0f)
        {
            d = 2.0;
        }
        scaleY = d;
    }
    ui->scaleYLineEdit->setText(QString::number(scaleY));
    ui->scaleYLineEdit->setPalette(*okBackground);
}

void ConversionDialog::on_delayLineEdit_editingFinished()
{
    if (!isReady) return;
    QString text = ui->delayLineEdit->text();
    int pos = 0;
    if (ui->delayLineEdit->validator()->validate(text, pos) == QValidator::Acceptable)
    {
        delayPTS = (int) subtitleProcessor->syncTimePTS(text.toDouble() * 90, fpsTrg);
    }
    ui->delayLineEdit->setText(QString::number(delayPTS / 90.0, 'g', 6));
    ui->delayLineEdit->setPalette(*okBackground);
}

void ConversionDialog::on_minTimeLineEdit_editingFinished()
{
    if (!isReady) return;
    QString text = ui->minTimeLineEdit->text();
    int pos = 0;
    if (ui->minTimeLineEdit->validator()->validate(text, pos) == QValidator::Acceptable)
    {
        minTimePTS = (int) subtitleProcessor->syncTimePTS(text.toDouble() * 90, fpsTrg);
    }
    ui->minTimeLineEdit->setText(QString::number(minTimePTS / 90.0, 'g', 6));
    ui->minTimeLineEdit->setPalette(*okBackground);
}

void ConversionDialog::on_sourceFramerateComboBox_currentIndexChanged(const QString &arg1)
{
    on_sourceFramerateComboBox_editTextChanged(arg1);
}

void ConversionDialog::on_sourceFramerateComboBox_editTextChanged(const QString &arg1)
{
    double d = subtitleProcessor->getFPS(arg1);
    if (d > 0)
    {
        ui->sourceFramerateComboBox->setPalette(*okBackground);
        fpsSrc = d;
    }
    else
    {
        ui->sourceFramerateComboBox->setPalette(*errorBackground);
    }
    fpsSrcCertain = false;
}

void ConversionDialog::on_sourceFramerateComboBox_editingFinished()
{
    if (!isReady) return;
    double d = subtitleProcessor->getFPS(ui->sourceFramerateComboBox->currentText());
    if (d > 0)
    {
        fpsSrc = d;
    }
    ui->sourceFramerateComboBox->setEditText(QString::number(fpsSrc, 'g', 6));
    ui->sourceFramerateComboBox->setPalette(*okBackground);
    fpsSrcCertain = false;
}

void ConversionDialog::on_targetFramerateComboBox_currentIndexChanged(const QString &arg1)
{
    on_targetFramerateComboBox_editTextChanged(arg1);
}

void ConversionDialog::on_targetFramerateComboBox_editTextChanged(const QString &arg1)
{
    double d = subtitleProcessor->getFPS(arg1);
    if (d > 0)
    {
        int newDelayPTS = (int)subtitleProcessor->syncTimePTS(delayPTS, fpsTrg);
        int newMinTimePTS = (int)subtitleProcessor->syncTimePTS(minTimePTS, fpsTrg);
        if (delayPTS != newDelayPTS || minTimePTS != newMinTimePTS)
        {
            ui->targetFramerateComboBox->setPalette(*warnBackground);
        }
        else
        {
            ui->targetFramerateComboBox->setPalette(*okBackground);
        }
        fpsTrg = d;
    }
    else
    {
        ui->targetFramerateComboBox->setPalette(*errorBackground);
    }
}

void ConversionDialog::on_targetFramerateComboBox_editingFinished()
{
    if (!isReady) return;
    QString text = ui->targetFramerateComboBox->currentText();
    double d = subtitleProcessor->getFPS(text);
    if (d > 0)
    {
        fpsTrg = d;
    }
    ui->targetFramerateComboBox->setEditText(QString::number(fpsTrg, 'g', 6));
    ui->targetFramerateComboBox->setPalette(*okBackground);

    delayPTS = (int)subtitleProcessor->syncTimePTS(delayPTS, fpsTrg);
    ui->delayLineEdit->setText(QString::number(delayPTS / 90.0, 'g', 6));

    minTimePTS = (int)subtitleProcessor->syncTimePTS(minTimePTS, fpsTrg);
    ui->minTimeLineEdit->setText(QString::number(minTimePTS / 90.0, 'g', 6));
}
