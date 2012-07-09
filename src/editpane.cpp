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

#include "editpane.h"

EditPane::EditPane(QWidget *parent, bool isLabel) :
    QLabel(parent),
    isLabel(isLabel)
{
}

void EditPane::mousePressEvent(QMouseEvent *event)
{
    if (allowSelection && event->button() == Qt::LeftButton)
    {
        int xPos = event->pos().x();// - pixmapXPos;
        int yPos = event->pos().y();// - pixmapYPos;
        selectStartX = (int)((xPos / xScaleCaption) + 0.5);
        selectStartY = (int)((yPos / yScaleCaption) + 0.5);
        leftButtonPressed = true;
        validSelection = false;
    }
}

void EditPane::mouseReleaseEvent(QMouseEvent *event)
{
    if (allowSelection && event->button() == Qt::LeftButton)
    {
        int xPos = event->pos().x();// - pixmapXPos;
        int yPos = event->pos().y();// - pixmapYPos;
        selectEndX = (int)((xPos / xScaleCaption) + 0.5);
        selectEndY = (int)((yPos / yScaleCaption) + 0.5);
        leftButtonPressed = false;
        if (selectStartX >= 0 && selectEndX > selectStartX && selectEndY > selectStartY)
        {
            validSelection = true;
        }
        update();
        emit selectionPerformed(validSelection);
    }
    else
    {
        emit onMouseClicked(event);
    }
}

void EditPane::mouseMoveEvent(QMouseEvent *event)
{
    if (leftButtonPressed)
    {
        int xPos = event->pos().x();// - pixmapXPos;
        int yPos = event->pos().y();// - pixmapYPos;
        selectEndX = (int)((xPos / xScaleCaption) + 0.5);
        selectEndY = (int)((yPos / yScaleCaption) + 0.5);
        if (selectStartX >= 0 && selectEndX > selectStartX && selectEndY > selectStartY)
        {
            validSelection = true;
        }
        update();
    }
}

void EditPane::paintEvent(QPaintEvent *event)
{
    int w, h;
    int x1, y1, rectWidth, rectHeight;

    w = this->size().width();
    h = this->size().height();

    if (isLabel)
    {
        rectWidth = w - (2 * this->margin());
        rectHeight = ((rectWidth * 9) + 8) / 16;
        if (rectHeight > h)
        {
            rectHeight = h - this->margin();
            rectWidth = ((rectHeight * 32) + 8) / 18;
        }
        y1 = ((h - rectHeight) + 1) / 2;
        x1 = ((w - rectWidth) + 1) / 2;
    }
    else
    {
        rectWidth = w;
        rectHeight = h;
        x1 = 0;
        y1 = 0;
    }

    int cinemascopeBarHeight = (rectHeight * cineBarFactor) + 0.5;

    QPainter painter(this);
    QLinearGradient gradient(x1, y1, rectWidth, rectHeight);
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

    if (!img.isNull() && img.width() != 0 && imgWidth > 0)
    {
        int wi = ((int)(imgWidth * sx) + 0.5);
        int hi = ((int)(imgHeight * sy) + 0.5);
        int xi = x1 + (int)((ofsX * sx) + 0.5);
        int yi = y1 + (int)((yCrop * sy) + 0.5);

        painter.setPen(Qt::green);
        painter.drawRect(xi, yi, wi - 1, hi - 1);
        painter.drawImage(QRect(xi, yi, wi, hi), img, img.rect());

        if (validSelection && !leftButtonPressed)
        {
            if (selectStartX >= (imgWidth + ofsX) || selectEndX <= ofsX ||
                selectStartY >= (imgHeight + yCrop) || selectEndY < yCrop)
            {
                validSelection = false;
            }
            else
            {
                if (selectStartX < ofsX)
                {
                    selectStartX = ofsX;
                }
                if (selectEndX >= imgWidth + ofsX)
                {
                    selectEndX = (imgWidth + ofsX) - 1;
                }
                if (selectStartY < yCrop)
                {
                    selectStartY = yCrop;
                }
                if (selectEndY >= imgHeight + yCrop)
                {
                    selectEndY = (imgHeight + yCrop) - 1;
                }
            }
        }

        if (cropOfsY > 0)
        {
            painter.setPen(Qt::red);
            int y = y1 + (int)((cropOfsY * sy) + 0.5);
            painter.drawLine(x1, y, (rectWidth + x1) - 1, y);
            y = ((y1 + rectHeight) - (int)((cropOfsY * sy) + 0.5)) - 1;
            painter.drawLine(x1, y, (rectWidth + x1) - 1, y);
        }

        if (validSelection)
        {
            painter.setPen(Qt::yellow);
            int topX = (int)((selectStartX * sx) + 0.5);
            int topY = (int)((selectStartY * sy) + 0.5);
            int drawWidth = (int)(((selectEndX - selectStartX) * sx) + 0.5);
            int drawHeight = (int)(((selectEndY - selectStartY) * sy) + 0.5);
            painter.drawRect(topX, topY, drawWidth, drawHeight);
        }

        if (excluded)
        {
            painter.setPen(Qt::red);
            painter.drawLine(x1, y1, (x1 + rectWidth) - 1, (y1 + rectHeight) - 1);
            painter.drawLine(x1 + rectWidth, y1, x1 - 1, (y1 + rectHeight) - 1);
        }
    }
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

void EditPane::setImage(QImage image, int width, int height)
{
    img = image;
    imgWidth = width;
    imgHeight = height;
    update();
}

QVector<int> EditPane::getSelection()
{
    QVector<int> selectionCoordinates;
    if (!allowSelection || !validSelection)
    {
        return selectionCoordinates;
    }
    selectionCoordinates.push_back(selectStartX - ofsX);
    selectionCoordinates.push_back(selectStartY - yCrop);
    selectionCoordinates.push_back(selectEndX - ofsX);
    selectionCoordinates.push_back(selectEndY - yCrop);
    return selectionCoordinates;
}
