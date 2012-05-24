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

#include "zoomableimagearea.h"
#include <QSize>
#include <QMessageBox>
#include <QPainter>
#include <QLinearGradient>
#include <QScrollArea>

ZoomableImageArea::ZoomableImageArea(QWidget *parent) :
    QLabel(parent)
{
}

void ZoomableImageArea::setImage(QImage* image)
{
    this->image = image;
}

void ZoomableImageArea::setZoomScale(int scale)
{
    if (scale != zoomScale && (image != 0 && !image->isNull()))
    {
        QRect currentSize = image->rect();
        scaleSize = QRect(currentSize.topLeft(), QSize(currentSize.width() * scale, currentSize.height() * scale));
        zoomScale = scale;
        update();
    }
}

void ZoomableImageArea::paintEvent(QPaintEvent *event)
{
    //TODO: add scaling code
    painter->begin(this);
    QLinearGradient gradient(0, 0, this->width(), this->height());
    gradient.setColorAt(0, Qt::blue);
    gradient.setColorAt(1, Qt::black);
    painter->fillRect(0, 0, this->width(), this->height(), gradient);
    if (image != 0 && !image->isNull())
    {
        painter->drawImage(0, 0, *image);
    }
    painter->end();
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
