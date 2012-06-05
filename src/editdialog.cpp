#include "editdialog.h"
#include "ui_editdialog.h"
#include "Subtitles/subtitleprocessor.h"
#include "Subtitles/subpicture.h"
#include "Tools/timeutil.h"
#include <QPalette>

EditDialog::EditDialog(QWidget *parent, SubtitleProcessor* subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);
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

    this->resize(minimumWidth + 36, minimumHeight + 280);
    ui->subtitleImage->setAllowSelection(true);

    okBackground = new QPalette(ui->durationLineEdit->palette());
    errorBackground = new QPalette();
    errorBackground->setColor(QPalette::Base, QColor(0xffe1acac));
    warnBackground = new QPalette();
    warnBackground->setColor(QPalette::Base, QColor(0xffffffc0));
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
    if (index < subtitleProcessor->getNumberOfFrames() - 1)
    {
        subPictureNext = subtitleProcessor->getSubPictureTrg(index + 1);
    }
    else
    {
        subPictureNext = 0;
    }

    subtitleProcessor->convertSup(index, index + 1, subtitleProcessor->getNumberOfFrames());
    subPicture = subtitleProcessor->getSubPictureTrg(index)->copy();
    image = subtitleProcessor->getTrgImagePatched(subPicture);

    if (!subPicture->erasePatch.isEmpty() && subPicture->erasePatch.size() > 0)
    {
        ui->undoErasePatchButton->setEnabled(true);
        ui->undoAllErasePatchesButton->setEnabled(true);
    }

    enableSliders = false;
    ui->horizontalSlider->setMaximum(subPicture->width);
    ui->horizontalSlider->setValue(subPicture->getOfsX());
    ui->verticalSlider->setMaximum(subPicture->height);
    ui->verticalSlider->setValue(subPicture->height - subPicture->getOfsY());
    enableSliders = true;

    ui->infoLabel->setText(QString("Frame %1 of %2").arg(QString::number(index + 1)).arg(subtitleProcessor->getNumberOfFrames()));
    ui->startTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->startTime));
    ui->endTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->endTime));
    ui->durationLineEdit->setText(QString::number((subPicture->endTime - subPicture->startTime) / 90.0, 'g', 5));

    ui->xOffsetLineEdit->setText(QString::number(subPicture->getOfsX()));
    ui->yOffsetLineEdit->setText(QString::number(subPicture->getOfsY()));

    ui->subtitleImage->setOffsets(subPicture->getOfsX(), subPicture->getOfsY());
    ui->subtitleImage->setDimension(subPicture->width, subPicture->height);
    ui->subtitleImage->setCropOfsY(subtitleProcessor->getCropOfsY());
    ui->subtitleImage->setImage(image, subPicture->getImageWidth(), subPicture->getImageHeight());
    ui->subtitleImage->update();
    ui->subtitleImage->setExcluded(subPicture->exclude);

    ui->excludeCheckBox->setChecked(subPicture->exclude);
    ui->forcedCaptionCheckBox->setChecked(subPicture->isForced);
}

void EditDialog::on_excludeCheckBox_toggled(bool checked)
{
    subPicture->exclude = checked;
    ui->subtitleImage->setExcluded(subPicture->exclude);
    ui->subtitleImage->update();
    setEdited(true);
}

void EditDialog::on_forcedCaptionCheckBox_toggled(bool checked)
{
    subPicture->isForced = checked;
    setEdited(true);
}

void EditDialog::on_minButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_maxButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_centerButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_topButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_bottomButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_cancelButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_saveButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_okButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_addErasePatchButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_undoErasePatchButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_undoAllErasePatchesButton_clicked()
{
    //TODO: implement
    throw 10;
}

void EditDialog::setEdited(bool edited)
{
    this->edited = edited;
    ui->saveButton->setEnabled(edited);
}

void EditDialog::on_startTimeLineEdit_textChanged(const QString &arg1)
{
    long t = subtitleProcessor->syncTimePTS(TimeUtil::timeStrToPTS(arg1), subtitleProcessor->getFPSTrg());
    if (t < 0 || t >= subPicture->endTime || subPicturePrevious != 0 && subPicturePrevious->endTime > t)
    {
        ui->startTimeLineEdit->setPalette(*errorBackground);
    }
    else
    {
        subPicture->startTime = t;
        ui->durationLineEdit->setText(QString::number((subPicture->endTime - subPicture->startTime)/ 90, 'g', 5));
        if (ui->startTimeLineEdit->text() != TimeUtil::ptsToTimeStr(subPicture->startTime))
        {
            ui->startTimeLineEdit->setPalette(*warnBackground);
        }
        else
        {
            ui->startTimeLineEdit->setPalette(*okBackground);
        }
        setEdited(true);
    }

//    if (t > subPicture->endTime)
//    {
//        t = subPicture->endTime - frameTime;
//    }
//    if (subPicturePrevious != 0 && subPicturePrevious->endTime > t)
//    {
//        t = subPicturePrevious->endTime + frameTime;
//    }
//    if (t >= 0)
//    {
//        subPicture->startTime = subtitleProcessor->syncTimePTS(t, subtitleProcessor->getFPSTrg());
//        ui->durationLineEdit->setText(QString::number((subPicture->endTime / subPicture->startTime) / 90, 'g', 5));
//        setEdited(true);
//    }
//    ui->startTimeLineEdit->setText(TimeUtil::ptsToTimeStr(subPicture->startTime));
//    isReady = true;
}

void EditDialog::on_endTimeLineEdit_textChanged(const QString &arg1)
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_durationLineEdit_textChanged(const QString &arg1)
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_xOffsetLineEdit_textChanged(const QString &arg1)
{
    //TODO: implement
    throw 10;
}

void EditDialog::on_yOffsetLineEdit_textChanged(const QString &arg1)
{
    //TODO: implement
    throw 10;
}
