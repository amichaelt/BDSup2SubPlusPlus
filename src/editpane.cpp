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

#include "editpane.h"

EditPane::EditPane(bool isLayoutPane, QWidget *parent) :
    QLabel(parent)
{
    Initialize(isLayoutPane);
}

EditPane::EditPane(QWidget *parent) :
    QLabel(parent)
{
    Initialize(true);
}

void EditPane::Initialize(bool isLayoutPane)
{
    layoutPane = isLayoutPane;
}

void EditPane::mousePressEvent(QMouseEvent *event)
{

}

void EditPane::paintEvent(QPaintEvent *event)
{
    //TODO: finish implementing
    int w, h;
    int x1, y1, rectWidth, rectHeight;

    w = size().width();
    h = size().height();

    rectWidth = w - 4;
    rectHeight = ((rectWidth * 9) + 8) / 16;

    if (rectHeight > h)
    {
        rectHeight = h - 2;
        rectWidth = ((rectHeight * 32) + 8) / 18;
    }
    y1 = ((h - rectHeight) + 1) / 2;
    x1 = ((w - rectWidth) + 1) / 2;

    int cinemascopeBarHeight = (rectHeight * (5.0 / 42)) + 0.5;

    QPainter painter(this);
    QLinearGradient gradient(0, 0, rectWidth, rectHeight);
    gradient.setColorAt(0, Qt::blue);
    gradient.setColorAt(1, Qt::black);
    painter.fillRect(x1, y1 + cinemascopeBarHeight, rectWidth, rectHeight - cinemascopeBarHeight, gradient);
    painter.fillRect(x1, y1, rectWidth, cinemascopeBarHeight, QBrush(Qt::black));
    painter.fillRect(x1, (y1 + rectHeight) - cinemascopeBarHeight, rectWidth, cinemascopeBarHeight, QBrush(Qt::black));

    yCrop = ofsY;
    if (yCrop < cropOfsY)
    {
        yCrop = cropOfsY;
    }
    else
    {
        int yMax = (height - imgHeight) - cropOfsY;
        if (yCrop > yMax)
        {
            yCrop = yMax;
        }
    }

    //draw scaled-down image
    double sx = (double)rectWidth / width;
    double sy = (double)rectHeight / height;
    xScaleCaption = sx;
    yScaleCaption = sy;

    if (imgWidth > 0 && img != 0)
    {
        int wi = ((int)(imgWidth * sx) + 0.5);
        int hi = ((int)(imgHeight * sy) + 0.5);
        int xi = x1 + (int)((ofsX * sx) + 0.5);
        int yi = y1 + (int)((yCrop * sy) + 0.5);

        painter.setPen(Qt::green);
        painter.drawRect(xi, yi, wi - 1, hi - 1);

        painter.drawImage(QRect(xi, yi, wi, hi), *img, img->rect());
    }

    //TODO: finish implementing
}

void EditPane::setOffsets(int x, int y)
{
    ofsX = x;
    if (y < cropOfsY)
    {
        ofsY = cropOfsY;
    }
    else
    {
        ofsY = y;
    }
}
