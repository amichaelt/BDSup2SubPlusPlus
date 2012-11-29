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

#include "zoomableimagearea.h"
#include <QSize>
#include <QPainter>
#include <QLinearGradient>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QScrollArea>
#else
#include <QScrollArea>
#endif

ZoomableImageArea::ZoomableImageArea(QWidget *parent) :
    QLabel(parent)
{
}

void ZoomableImageArea::setImage(QImage inImage)
{
    image = inImage;
    if (zoomScale == 0)
    {
        setZoomScale(1);
    }
    else
    {
        setZoomScale(zoomScale);
    }
}

void ZoomableImageArea::updateImage()
{
    setZoomScale(zoomScale);
}

void ZoomableImageArea::setZoomScale(int scale)
{
    if (scale != zoomScale)
    {
        zoomScale = scale;
    }
    if (!image.isNull() && image.width() != 0)
    {
        QRect target = image.rect();
        target.setWidth(image.width() * zoomScale);
        target.setHeight(image.height() * zoomScale);

        QRect visibleArea = this->parentWidget()->visibleRegion().boundingRect();

        int drawHeight = target.height() > visibleArea.height() ? target.height() : visibleArea.height();
        int drawWidth = target.width() > visibleArea.width() ? target.width() : visibleArea.width();


        drawPixmap.reset(new QPixmap(drawWidth, drawHeight));
        drawPixmap->fill();

        painter->begin(drawPixmap.data());
        QLinearGradient gradient(0, 0, drawPixmap->width(), drawPixmap->height());
        gradient.setColorAt(0, Qt::blue);
        gradient.setColorAt(1, Qt::black);
        painter->fillRect(0, 0, drawPixmap->width(), drawPixmap->height(), gradient);
        painter->drawImage(target, image, image.rect());
        painter->end();
        this->setPixmap(*drawPixmap);
    }
}

void ZoomableImageArea::paintEvent(QPaintEvent *event)
{
    if (image.isNull() || image.width() == 0)
    {
        painter->begin(this);
        QLinearGradient gradient(0, 0, this->width(), this->height());
        gradient.setColorAt(0, Qt::blue);
        gradient.setColorAt(1, Qt::black);
        painter->fillRect(0, 0, this->width(), this->height(), gradient);
        painter->end();
        originalSize = this->size();
    }
    else
    {
        painter->begin(this);
        painter->drawPixmap(0, 0, *drawPixmap);
        painter->end();
    }
}

void ZoomableImageArea::mousePressEvent(QMouseEvent *event)
{
    int scale = zoomScale;
    if (event->button() == Qt::LeftButton)
    {
        if (scale < 8)
        {
            ++scale;
        }
    }
    else
    {
        if (scale > 1)
        {
            --scale;
        }
    }
    if (scale != zoomScale)
    {
        setZoomScale(scale);
    }
}
