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

#include "framepalettedialog.h"
#include "ui_framepalettedialog.h"
#include "Subtitles/subtitleprocessor.h"

#include <QPixmap>

FramePaletteDialog::FramePaletteDialog(QWidget *parent, SubtitleProcessor* subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::FramePaletteDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    this->subtitleProcessor = subtitleProcessor;
    Palette* palette = this->subtitleProcessor->getCurrentSrcDVDPalette();
    ui->color1ComboBox->blockSignals(true);
    ui->color2ComboBox->blockSignals(true);
    ui->color3ComboBox->blockSignals(true);
    ui->color4ComboBox->blockSignals(true);
    ui->alpha1ComboBox->blockSignals(true);
    ui->alpha2ComboBox->blockSignals(true);
    ui->alpha3ComboBox->blockSignals(true);
    ui->alpha4ComboBox->blockSignals(true);
    for (int i = 0; i < palette->getSize(); ++i)
    {
        QPixmap* pixmap = new QPixmap(12, 12);
        pixmap->fill(QColor(palette->getARGB(i)));
        colorIcons.insert(i, QIcon(*pixmap));
        ui->color1ComboBox->addItem(colorIcons[i], colorNames[i]);
        ui->color2ComboBox->addItem(colorIcons[i], colorNames[i]);
        ui->color3ComboBox->addItem(colorIcons[i], colorNames[i]);
        ui->color4ComboBox->addItem(colorIcons[i], colorNames[i]);
        ui->alpha1ComboBox->addItem(colorNames[i]);
        ui->alpha2ComboBox->addItem(colorNames[i]);
        ui->alpha3ComboBox->addItem(colorNames[i]);
        ui->alpha4ComboBox->addItem(colorNames[i]);
    }
    ui->color1ComboBox->blockSignals(false);
    ui->color2ComboBox->blockSignals(false);
    ui->color3ComboBox->blockSignals(false);
    ui->color4ComboBox->blockSignals(false);
    ui->alpha1ComboBox->blockSignals(false);
    ui->alpha2ComboBox->blockSignals(false);
    ui->alpha3ComboBox->blockSignals(false);
    ui->alpha4ComboBox->blockSignals(false);
}

FramePaletteDialog::~FramePaletteDialog()
{
    delete ui;
}

void FramePaletteDialog::setIndex(int idx)
{
    index = idx;

    QVector<int> a = subtitleProcessor->getFrameAlpha(index);
    QVector<int> p = subtitleProcessor->getFramePal(index);

    if (!a.isEmpty())
    {
        alpha = a;
    }
    if (!p.isEmpty())
    {
        pal = p;
    }

    ui->color1ComboBox->setCurrentIndex(pal[0]);
    ui->color2ComboBox->setCurrentIndex(pal[1]);
    ui->color3ComboBox->setCurrentIndex(pal[2]);
    ui->color4ComboBox->setCurrentIndex(pal[3]);

    ui->alpha1ComboBox->setCurrentIndex(alpha[0]);
    ui->alpha2ComboBox->setCurrentIndex(alpha[1]);
    ui->alpha3ComboBox->setCurrentIndex(alpha[2]);
    ui->alpha4ComboBox->setCurrentIndex(alpha[3]);
}

void FramePaletteDialog::on_color1ComboBox_currentIndexChanged(int index)
{
    pal[0] = index;
}

void FramePaletteDialog::on_color2ComboBox_currentIndexChanged(int index)
{
    pal[1] = index;
}

void FramePaletteDialog::on_color3ComboBox_currentIndexChanged(int index)
{
    pal[2] = index;
}

void FramePaletteDialog::on_color4ComboBox_currentIndexChanged(int index)
{
    pal[3] = index;
}

void FramePaletteDialog::on_alpha1ComboBox_currentIndexChanged(int index)
{
    alpha[0] = index;
}

void FramePaletteDialog::on_alpha2ComboBox_currentIndexChanged(int index)
{
    alpha[1] = index;
}

void FramePaletteDialog::on_alpha3ComboBox_currentIndexChanged(int index)
{
    alpha[2] = index;
}

void FramePaletteDialog::on_alpha4ComboBox_currentIndexChanged(int index)
{
    alpha[3] = index;
}

void FramePaletteDialog::on_resetAllButton_clicked()
{
    for (int j = 0; j < subtitleProcessor->getNumberOfFrames(); ++j)
    {
        QVector<int> originalAlpha = subtitleProcessor->getOriginalFrameAlpha(j);
        QVector<int> originalPalette = subtitleProcessor->getOriginalFramePal(j);
        QVector<int> a = subtitleProcessor->getFrameAlpha(j);
        QVector<int> p = subtitleProcessor->getFramePal(j);

        if (!a.isEmpty() && !originalAlpha.isEmpty())
        {
            for (int i = 0; i < a.size(); ++i)
            {
                a.replace(i, originalAlpha[i]);
            }
        }
        if (!p.isEmpty() && !originalPalette.isEmpty())
        {
            for (int i = 0; i < p.size(); ++i)
            {
                p.replace(i, originalPalette[i]);
            }
        }
    }
}

void FramePaletteDialog::on_setAllButton_clicked()
{
    for (int j = 0; j < subtitleProcessor->getNumberOfFrames(); ++j)
    {
        QVector<int> a = subtitleProcessor->getFrameAlpha(j);
        QVector<int> p = subtitleProcessor->getFramePal(j);
        if (!a.isEmpty())
        {
            for (int i = 0; i < a.size(); ++i)
            {
                a.replace(i, alpha[i]);
            }
        }
        if (!p.isEmpty())
        {
            for (int i = 0; i < p.size(); ++i)
            {
                p.replace(i, pal[i]);
            }
        }
    }
}

void FramePaletteDialog::on_cancelButton_clicked()
{
    reject();
}

void FramePaletteDialog::on_resetButton_clicked()
{
    QVector<int> originalAlpha = subtitleProcessor->getOriginalFrameAlpha(index);
    QVector<int> originalPalette = subtitleProcessor->getOriginalFramePal(index);
    QVector<int> a = subtitleProcessor->getFrameAlpha(index);
    QVector<int> p = subtitleProcessor->getFramePal(index);

    if (!a.isEmpty() && !originalAlpha.isEmpty())
    {
        for (int i = 0; i < a.size(); ++i)
        {
            a.replace(i, originalAlpha[i]);
        }
    }
    if (!p.isEmpty() && !originalPalette.isEmpty())
    {
        for (int i = 0; i < p.size(); ++i)
        {
            p.replace(i, originalPalette[i]);
        }
    }
    setIndex(index);
}

void FramePaletteDialog::on_okButton_clicked()
{
    QVector<int> a = subtitleProcessor->getFrameAlpha(index);
    QVector<int> p = subtitleProcessor->getFramePal(index);
    if (!a.isEmpty())
    {
        for (int i = 0; i < a.size(); ++i)
        {
            a.replace(i, alpha[i]);
        }
    }
    if (!p.isEmpty())
    {
        for (int i = 0; i < p.size(); ++i)
        {
            p.replace(i, pal[i]);
        }
    }
    accept();
}
