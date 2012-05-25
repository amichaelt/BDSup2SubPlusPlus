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

#ifndef EDITPANE_H
#define EDITPANE_H

#include <QtGui>

class EditPane : public QLabel
{
    Q_OBJECT
public:
    explicit EditPane(bool isLayoutPane, QWidget *parent = 0);
    explicit EditPane(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

    void setDimension(int width, int height) { this->width = width; this->height = height; }
    void setOffsets(int x, int y);
    void setCropOfsY(int ofs) { cropOfsY = ofs; }
    void setImage(QImage* image, int width, int height);
    void setExcluded(bool excluded) { this->excluded = excluded; }

private:
    QImage* img = 0;
    QColor color1 = Qt::blue;
    QColor color2 = Qt::black;
    int width = 1920;
    int height = 1080;
    int ofsX;
    int ofsY;
    int imgWidth = 0;
    int imgHeight = 0;
    double screenRatioIn = 21.0 / 9;
    double cineBarFactor = 5.0 / 42;
    static constexpr double screenRatio = 16.0 / 9;
    int cropOfsY = 0;
    int inset = 2;
    bool layoutPane;
    int selectStartX = -1;
    int selectEndX;
    int selectStartY;
    int selectEndY;
    bool allowSelection = false;
    bool leftButtonPressed;
    bool excluded;
    double xScaleCaption;
    double yScaleCaption;
    int yCrop;

    void Initialize(bool isLayoutPane);
    
signals:
    
public slots:
    
};

#endif // EDITPANE_H
