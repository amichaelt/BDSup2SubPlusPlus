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

#include "editdialog.h"
#include "ui_editdialog.h"
#include "Subtitles/subtitleprocessor.h"
#include "Subtitles/subpicture.h"
#include "Tools/timeutil.h"
#include "types.h"

#include <QPalette>
#include <QKeyEvent>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QRegExpValidator>

EditDialog::EditDialog(QWidget *parent, SubtitleProcessor* subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);
    ui->subtitleImage->setIsLabel(false);
    connect(ui->subtitleImage, SIGNAL(selectionPerformed(bool)), this, SLOT(onSelectionPerformed(bool)));
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    this->subtitleProcessor = subtitleProcessor;

    Resolution resolution = subtitleProcessor->getOutputResolution();

    switch ((int)resolution)
    {
    case (int)Resolution::PAL:
    case (int)Resolution::NTSC:
    {
        minimumWidth = 720;
        minimumHeight = 405;
    } break;
    case (int)Resolution::HD_720:
    case (int)Resolution::HD_1440x1080:
    case (int)Resolution::HD_1080:
    {
        minimumWidth = 640;
        minimumHeight = 320;
    } break;
    }

    frameTime = (int)(90000 / subtitleProcessor->getFPSTrg());
    durationValidator = new QDoubleValidator;
    ui->durationLineEdit->setValidator(durationValidator);
    xOffsetValidator = new QIntValidator;
    ui->xOffsetLineEdit->setValidator(xOffsetValidator);
    yOffsetValidator = new QIntValidator;
    ui->yOffsetLineEdit->setValidator(yOffsetValidator);
    startTimeValidator = new QRegExpValidator(TimeUtil::getTimePattern());
    ui->startTimeLineEdit->setValidator(startTimeValidator);
    endTimeValidator = new QRegExpValidator(TimeUtil::getTimePattern());
    ui->endTimeLineEdit->setValidator(endTimeValidator);

    this->resize(minimumWidth + 36, minimumHeight + 280);
    ui->subtitleImage->setAllowSelection(true);

    okBackground = new QPalette(ui->durationLineEdit->palette());
    errorBackground = new QPalette();
    errorBackground->setColor(QPalette::Base, QColor(0xffe1acac));
    warnBackground = new QPalette();
    warnBackground->setColor(QPalette::Base, QColor(0xffffffc0));

    isReady = true;
}

EditDialog::~EditDialog()
{
    delete ui;
}

void EditDialog::setIndex(int value)
{
    index = value;
    if (index > 0)
    {
        subPicturePrevious = subtitleProcessor->getSubPictureTrg(index - 1);
    }
    else
    {
        subPicturePrevious = 0;
    }
    int num = subtitleProcessor->getNumberOfFrames();
    if (index < num - 1)
    {
        subPictureNext = subtitleProcessor->getSubPictureTrg(index + 1);
    }
    else
    {
        subPictureNext = 0;
    }

    try
    {
        subtitleProcessor->convertSup(index, index + 1, subtitleProcessor->getNumberOfFrames());
        subPicture = subtitleProcessor->getSubPictureTrg(index)->copy();
        image = subtitleProcessor->getTrgImagePatched(subPicture);
    }
    catch (QString e)
    {
        subtitleProcessor->printError(QString("ERROR: " + e));
        QMessageBox::warning(this, "Error!", e);
        return;
    }

    if (!subPicture->erasePatch.isEmpty())
    {
        ui->undoErasePatchButton->setEnabled(true);
        ui->undoAllErasePatchesButton->setEnabled(true);
    }

    ui->horizontalSlider->blockSignals(true);
    ui->verticalSlider->blockSignals(true);

    ui->horizontalSlider->setMaximum(subPicture->width());
    ui->horizontalSlider->setValue(subPicture->getOfsX());

    ui->verticalSlider->setMaximum(subPicture->height());
    ui->verticalSlider->setValue(subPicture->height() - subPicture->getOfsY());

    ui->horizontalSlider->blockSignals(false);
    ui->verticalSlider->blockSignals(false);

    ui->startTimeLineEdit->blockSignals(true);
    ui->endTimeLineEdit->blockSignals(true);
    ui->durationLineEdit->blockSignals(true);
    ui->xOffsetLineEdit->blockSignals(true);
    ui->yOffsetLineEdit->blockSignals(true);

    ui->infoLabel->setText(QString("Frame %1 of %2").arg(QString::number(index + 1)).arg(subtitleProcessor->getNumberOfFrames()));
    ui->startTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->startTime()));
    ui->endTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->endTime()));
    ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime()) / 90.0, 'g', 6));

    ui->xOffsetLineEdit->setText(QString::number(subPicture->getOfsX()));
    ui->yOffsetLineEdit->setText(QString::number(subPicture->getOfsY()));

    ui->startTimeLineEdit->blockSignals(false);
    ui->endTimeLineEdit->blockSignals(false);
    ui->durationLineEdit->blockSignals(false);
    ui->xOffsetLineEdit->blockSignals(false);
    ui->yOffsetLineEdit->blockSignals(false);

    ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
    ui->subtitleImage->setDimension(subPicture->width(), subPicture->height());
    ui->subtitleImage->setCropOfsY(subtitleProcessor->getCropOfsY());
    ui->subtitleImage->setImage(image, subPicture->getImageWidth(), subPicture->getImageHeight());
    ui->subtitleImage->update();
    ui->subtitleImage->setExcluded(subPicture->exclude());

    ui->excludeCheckBox->setChecked(subPicture->exclude());
    ui->forcedCaptionCheckBox->setChecked(subPicture->isForced());
}

void EditDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if (ui->startTimeLineEdit->hasFocus())
        {
            on_startTimeLineEdit_editingFinished();
            return;
        }
        if (ui->endTimeLineEdit->hasFocus())
        {
            on_endTimeLineEdit_editingFinished();
            return;
        }
        if (ui->durationLineEdit->hasFocus())
        {
            on_durationLineEdit_editingFinished();
            return;
        }
        if (ui->xOffsetLineEdit->hasFocus())
        {
            on_xOffsetLineEdit_editingFinished();
            return;
        }
        if (ui->yOffsetLineEdit->hasFocus())
        {
            on_yOffsetLineEdit_editingFinished();
            return;
        }
    }
    QDialog::keyPressEvent(event);
}

void EditDialog::onSelectionPerformed(bool validSelection)
{
    ui->addErasePatchButton->setEnabled(validSelection);
}

void EditDialog::on_excludeCheckBox_toggled(bool checked)
{
    subPicture->setExclude(checked);
    ui->subtitleImage->setExcluded(subPicture->exclude());
    ui->subtitleImage->update();
    setEdited(true);
}

void EditDialog::on_forcedCaptionCheckBox_toggled(bool checked)
{
    subPicture->setForced(checked);
    setEdited(true);
}

void EditDialog::on_minButton_clicked()
{
    quint64 t = subtitleProcessor->getMinTimePTS();
    if (t >= 0)
    {
        t += subPicture->startTime();
        if (subPictureNext != 0 && subPictureNext->startTime() < t)
        {
            t = subPictureNext->startTime();
        }
        subPicture->setEndTime(subtitleProcessor->syncTimePTS(t, subtitleProcessor->getFPSTrg()));
        ui->endTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->endTime()));
        setEdited(true);
    }
    ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime()) / 90.0, 'g', 6));
}

void EditDialog::on_maxButton_clicked()
{
    quint64 t;
    if (subPictureNext != 0)
    {
        t = subPictureNext->startTime();
    }
    else
    {
        t = subPicture->endTime() + (10000 * 90); // 10 seconds
    }
    subPicture->setEndTime(subtitleProcessor->syncTimePTS(t, subtitleProcessor->getFPSTrg()));
    ui->endTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->endTime()));
    ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime()) / 90.0, 'g', 6));
    setEdited(true);
}

void EditDialog::on_centerButton_clicked()
{
    subPicture->setOfsX((subPicture->width() - subPicture->getImageWidth()) / 2);

    ui->horizontalSlider->blockSignals(true);

    ui->horizontalSlider->setValue(subPicture->getOfsX());
    ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
    ui->subtitleImage->update();
    ui->xOffsetLineEdit->setText(QString::number(subPicture->getOfsX()));
    setEdited(true);

    ui->horizontalSlider->blockSignals(false);

}

void EditDialog::on_topButton_clicked()
{
    int cineH = (subPicture->height() * 5) / 42;
    int y = cineH - subPicture->getImageHeight();
    if (y < 10)
    {
        y = 10;
    }
    if (y < subtitleProcessor->getCropOfsY())
    {
        y = subtitleProcessor->getCropOfsY();
    }
    ui->verticalSlider->blockSignals(true);

    subPicture->setOfsY(y);
    ui->verticalSlider->setValue(subPicture->height() - subPicture->getOfsY());
    ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
    ui->subtitleImage->update();
    ui->yOffsetLineEdit->setText(QString::number(subPicture->getOfsY()));
    setEdited(true);

    ui->verticalSlider->blockSignals(false);
}

void EditDialog::on_bottomButton_clicked()
{
    int cineH = (subPicture->height() * 5) / 42;
    int y = subPicture->height() - cineH;
    if ((y + subPicture->getImageHeight()) > (subPicture->height() - subtitleProcessor->getCropOfsY()))
    {
        y = (subPicture->height() - subPicture->getImageHeight()) - 10;
    }

    ui->verticalSlider->blockSignals(true);

    subPicture->setOfsY(y);
    ui->verticalSlider->setValue(subPicture->height() - subPicture->getOfsY());
    ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
    ui->subtitleImage->update();
    setEdited(true);

    ui->verticalSlider->blockSignals(false);
}

void EditDialog::on_cancelButton_clicked()
{
    reject();
}

void EditDialog::on_saveButton_clicked()
{
    store();
    setEdited(false);
}

void EditDialog::on_okButton_clicked()
{
    if (edited)
    {
        store();
    }
    accept();
}

void EditDialog::on_addErasePatchButton_clicked()
{
    QVector<int> selectionCoordinates = ui->subtitleImage->getSelection();
    if (!selectionCoordinates.isEmpty())
    {
        ErasePatch* ep = new ErasePatch(selectionCoordinates[0], selectionCoordinates[1],
                                       (selectionCoordinates[2] - selectionCoordinates[0]) + 1,
                                       (selectionCoordinates[3] - selectionCoordinates[1]) + 1);
        subPicture->erasePatch.push_back(ep);

        ui->undoErasePatchButton->setEnabled(true);
        ui->undoAllErasePatchesButton->setEnabled(true);

        image = subtitleProcessor->getTrgImagePatched(subPicture);
        ui->subtitleImage->setImage(image, subPicture->getImageWidth(), subPicture->getImageHeight());

        setEdited(true);
    }
    ui->addErasePatchButton->setEnabled(false);
    ui->subtitleImage->removeSelection();
    ui->subtitleImage->update();
}

void EditDialog::on_undoErasePatchButton_clicked()
{
    if (!subPicture->erasePatch.isEmpty())
    {
        subPicture->erasePatch.remove(subPicture->erasePatch.size() - 1);
        if (subPicture->erasePatch.size() == 0)
        {
            ui->undoErasePatchButton->setEnabled(false);
            ui->undoAllErasePatchesButton->setEnabled(false);
        }
        image = subtitleProcessor->getTrgImagePatched(subPicture);
        ui->subtitleImage->setImage(image, subPicture->getImageWidth(), subPicture->getImageHeight());
        ui->subtitleImage->update();
        setEdited(true);
    }
}

void EditDialog::on_undoAllErasePatchesButton_clicked()
{
    subPicture->erasePatch.clear();
    image = subtitleProcessor->getTrgImagePatched(subPicture);
    ui->subtitleImage->setImage(image, subPicture->getImageWidth(), subPicture->getImageHeight());
    ui->subtitleImage->update();
    setEdited(true);
    ui->undoErasePatchButton->setEnabled(false);
    ui->undoAllErasePatchesButton->setEnabled(false);
}

void EditDialog::setEdited(bool edited)
{
    this->edited = edited;
    ui->saveButton->setEnabled(edited);
}

void EditDialog::store()
{
    SubPicture* subPic = subtitleProcessor->getSubPictureTrg(index);
    subPic->setEndTime(subPicture->endTime());
    subPic->setStartTime(subPicture->startTime());
    subPic->setOfsX(subPicture->getOfsX());
    subPic->setOfsY(subPicture->getOfsY());
    subPic->setForced(subPicture->isForced());
    subPic->setExclude(subPicture->exclude());
    subPic->erasePatch = subPicture->erasePatch;
}

void EditDialog::on_startTimeLineEdit_textChanged(const QString &arg1)
{
    if (!isReady) return;

    bool ok;
    quint64 timestamp = TimeUtil::timeStrToPTS(arg1, &ok);
    timestamp = ok ? timestamp : -1;
    quint64 t = subtitleProcessor->syncTimePTS(timestamp, subtitleProcessor->getFPSTrg());
    if (t < 0 || t >= subPicture->endTime() || (subPicturePrevious != 0 && subPicturePrevious->endTime() > t))
    {
        ui->startTimeLineEdit->setPalette(*errorBackground);
    }
    else
    {
        subPicture->setStartTime(t);
        ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime())/ 90.0, 'g', 6));
        if (ui->startTimeLineEdit->text() != TimeUtil::ptsToTimeStr(subPicture->startTime()))
        {
            ui->startTimeLineEdit->setPalette(*warnBackground);
        }
        else
        {
            ui->startTimeLineEdit->setPalette(*okBackground);
        }
        setEdited(true);
    }
}

void EditDialog::on_endTimeLineEdit_textChanged(const QString &arg1)
{
    if (!isReady) return;

    bool ok;
    quint64 timestamp = TimeUtil::timeStrToPTS(arg1, &ok);
    timestamp = ok ? timestamp : -1;
    quint64 t = subtitleProcessor->syncTimePTS(timestamp, subtitleProcessor->getFPSTrg());
    if (t < 0 || t <= subPicture->startTime() || (subPictureNext != 0 && subPictureNext->startTime() < t))
    {
        ui->endTimeLineEdit->setPalette(*errorBackground);
    }
    else
    {
        subPicture->setEndTime(t);
        ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime())/ 90.0, 'g', 6));
        if (ui->endTimeLineEdit->text() != TimeUtil::ptsToTimeStr(subPicture->endTime()))
        {
            ui->endTimeLineEdit->setPalette(*warnBackground);
        }
        else
        {
            ui->endTimeLineEdit->setPalette(*okBackground);
        }
        setEdited(true);
    }
}

void EditDialog::on_durationLineEdit_textChanged(const QString &arg1)
{
    if (!isReady) return;

    quint64 t = (long)(arg1.toDouble() * 90);
    if (t < frameTime)
    {
        ui->durationLineEdit->setPalette(*errorBackground);
    }
    else
    {
        t += subPicture->startTime();
        if (subPictureNext != 0 && subPictureNext->startTime() < t)
        {
            t = subPictureNext->startTime();
        }
        subPicture->setEndTime(subtitleProcessor->syncTimePTS(subPicture->endTime(), subtitleProcessor->getFPSTrg()));
        setEdited(true);
        if (ui->durationLineEdit->text() != QString::number((subPicture->endTime() - subPicture->startTime()) / 90.0, 'g', 6))
        {
            ui->durationLineEdit->setPalette(*warnBackground);
        }
        else
        {
            ui->durationLineEdit->setPalette(*okBackground);
        }
    }
}

void EditDialog::on_xOffsetLineEdit_textChanged(const QString &arg1)
{
    if (!isReady) return;

    int x = ui->xOffsetLineEdit->text().toInt();
    if (x > subPicture->width() - subPicture->getImageWidth())
    {
        ui->xOffsetLineEdit->setPalette(*errorBackground);
    }
    else
    {
        if (x != subPicture->getOfsX())
        {
            ui->horizontalSlider->blockSignals(true);
            subPicture->setOfsX(x);
            ui->horizontalSlider->setValue(subPicture->getOfsX());
            ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
            ui->subtitleImage->update();
            setEdited(true);
            ui->horizontalSlider->blockSignals(false);
        }
        ui->xOffsetLineEdit->setPalette(*okBackground);
    }
}

void EditDialog::on_yOffsetLineEdit_textChanged(const QString &arg1)
{
    if (!isReady) return;

    int y = ui->yOffsetLineEdit->text().toInt();
    if (y < subtitleProcessor->getCropOfsY() || y > ((subPicture->height() - subPicture->getImageHeight()) - subtitleProcessor->getCropOfsY()))
    {
        ui->yOffsetLineEdit->setPalette(*errorBackground);
    }
    else
    {
        if (y != subPicture->getOfsY())
        {
            ui->verticalSlider->blockSignals(true);
            subPicture->setOfsY(y);
            ui->verticalSlider->setValue(subPicture->height() - subPicture->getOfsY());
            ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
            ui->subtitleImage->update();
            setEdited(true);
            ui->verticalSlider->blockSignals(false);
        }
        ui->yOffsetLineEdit->setPalette(*okBackground);
    }
}

void EditDialog::on_verticalSlider_valueChanged(int value)
{
    if (!isReady) return;

    int y = subPicture->height() - value;

    if (y < subtitleProcessor->getCropOfsY())
    {
        y = subtitleProcessor->getCropOfsY();
    }
    else if (y > ((subPicture->height() - subPicture->getImageHeight()) - subtitleProcessor->getCropOfsY()))
    {
        y = (subPicture->height() - subPicture->getImageHeight()) - subtitleProcessor->getCropOfsY();
    }

    if (y != subPicture->getOfsY())
    {
        subPicture->setOfsY(y);
        ui->yOffsetLineEdit->setText(QString::number(subPicture->getOfsY()));
        ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
        ui->subtitleImage->setScreenRatio(21.0 / 9);
        ui->subtitleImage->update();
        setEdited(true);
    }
}

void EditDialog::on_horizontalSlider_valueChanged(int value)
{
    if (!isReady) return;

    int x = value;

    if (x < 0)
    {
        x = 0;
    }
    else if (x > subPicture->width() - subPicture->getImageWidth())
    {
        x = subPicture->width() - subPicture->getImageWidth();
    }

    if (x != subPicture->getOfsX())
    {
        subPicture->setOfsX(x);
        ui->xOffsetLineEdit->setText(QString::number(subPicture->getOfsX()));
        ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
        ui->subtitleImage->update();
        setEdited(true);
    }
}

void EditDialog::on_startTimeLineEdit_editingFinished()
{
    bool ok;
    quint64 timestamp = TimeUtil::timeStrToPTS(ui->startTimeLineEdit->text(), &ok);
    timestamp = ok ? timestamp : -1;
    quint64 t = subtitleProcessor->syncTimePTS(timestamp, subtitleProcessor->getFPSTrg());
    if (t > subPicture->endTime())
    {
        t = subPicture->endTime() - frameTime;
    }
    if (subPicturePrevious != 0 && subPicturePrevious->endTime() > t)
    {
        t = subPicturePrevious->endTime() + frameTime;
    }
    if (t >= 0)
    {
        subPicture->setStartTime(subtitleProcessor->syncTimePTS(t, subtitleProcessor->getFPSTrg()));
        ui->durationLineEdit->blockSignals(true);

        ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime()) / 90.0, 'g', 6));

        ui->durationLineEdit->blockSignals(false);
        setEdited(true);
    }

    ui->startTimeLineEdit->blockSignals(true);

    ui->startTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->startTime()));
    ui->startTimeLineEdit->setPalette(*okBackground);

    ui->startTimeLineEdit->blockSignals(false);
}

void EditDialog::on_endTimeLineEdit_editingFinished()
{
    bool ok;
    quint64 timestamp = TimeUtil::timeStrToPTS(ui->endTimeLineEdit->text(), &ok);
    timestamp = ok ? timestamp : -1;
    quint64 t = subtitleProcessor->syncTimePTS(timestamp, subtitleProcessor->getFPSTrg());
    if (t <= subPicture->startTime())
    {
        t = subPicture->startTime() + frameTime;
    }
    if (subPictureNext != 0 && subPictureNext->startTime() < t)
    {
        t = subPictureNext->startTime();
    }
    if (t >= 0)
    {
        subPicture->setEndTime(subtitleProcessor->syncTimePTS(t, subtitleProcessor->getFPSTrg()));
        ui->durationLineEdit->blockSignals(true);

        ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime()) / 90.0, 'g', 6));

        ui->durationLineEdit->blockSignals(false);
        setEdited(true);
    }

    ui->endTimeLineEdit->blockSignals(true);

    ui->endTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->startTime()));
    ui->endTimeLineEdit->setPalette(*okBackground);

    ui->endTimeLineEdit->blockSignals(false);
}

void EditDialog::on_durationLineEdit_editingFinished()
{
    quint64 t = (long)(ui->durationLineEdit->text().toDouble() * 90);
    if (t >= 0 && t < frameTime)
    {
        t = frameTime;
    }
    if (t > 0)
    {
        t += subPicture->startTime();
        if (subPictureNext != 0 && subPictureNext->startTime() < t)
        {
            t = subPictureNext->startTime();
        }
        subPicture->setEndTime(subtitleProcessor->syncTimePTS(t, subtitleProcessor->getFPSTrg()));
        ui->endTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->endTime()));
        setEdited(true);
    }
    ui->durationLineEdit->setText(QString::number((subPicture->endTime() - subPicture->startTime()) / 90.0, 'g', 6));
    ui->durationLineEdit->setPalette(*okBackground);
}

void EditDialog::on_xOffsetLineEdit_editingFinished()
{
    int x = ui->xOffsetLineEdit->text().toInt();
    if (x < 0)
    {
        x = 0;
    }
    else if (x > subPicture->width() - subPicture->getImageWidth())
    {
        x = subPicture->width() - subPicture->getImageWidth();
    }

    if (x != subPicture->getOfsX())
    {
        ui->horizontalSlider->blockSignals(true);
        subPicture->setOfsX(x);
        ui->horizontalSlider->setValue(subPicture->getOfsX());
        ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
        ui->subtitleImage->update();
        setEdited(true);
        ui->horizontalSlider->blockSignals(false);
    }
    ui->xOffsetLineEdit->setText(QString::number(subPicture->getOfsX()));
    ui->xOffsetLineEdit->setPalette(*okBackground);
}

void EditDialog::on_yOffsetLineEdit_editingFinished()
{
    int y = ui->yOffsetLineEdit->text().toInt();
    if (y < subtitleProcessor->getCropOfsY())
    {
        y = subtitleProcessor->getCropOfsY();
    }
    else if (y > ((subPicture->height() - subPicture->getImageHeight()) - subtitleProcessor->getCropOfsY()))
    {
        y = (subPicture->height() - subPicture->getImageHeight()) - subtitleProcessor->getCropOfsY();
    }

    if (y != subPicture->getOfsY())
    {
        ui->verticalSlider->blockSignals(true);
        subPicture->setOfsY(y);
        ui->verticalSlider->setValue(subPicture->height() - subPicture->getOfsY());
        ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
        ui->subtitleImage->update();
        setEdited(true);
        ui->verticalSlider->blockSignals(false);
    }
    ui->yOffsetLineEdit->setText(QString::number(subPicture->getOfsY()));
    ui->yOffsetLineEdit->setPalette(*okBackground);
}

void EditDialog::on_storePreviousButton_clicked()
{
    if (edited)
    {
        store();
    }
    if (index > 0)
    {
        setIndex(index - 1);
        setEdited(false);
    }
}

void EditDialog::on_storeNextButton_clicked()
{
    if (edited)
    {
        store();
    }
    if (index < subtitleProcessor->getNumberOfFrames() - 1)
    {
        setIndex(index + 1);
        setEdited(false);
    }
}

void EditDialog::on_previousButton_clicked()
{
    if (index > 0)
    {
        setIndex(index - 1);
        setEdited(false);
    }
}

void EditDialog::on_nextButton_clicked()
{
    if (index < subtitleProcessor->getNumberOfFrames() - 1)
    {
        setIndex(index + 1);
        setEdited(false);
    }
}
