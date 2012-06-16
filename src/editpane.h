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

#ifndef EDITPANE_H
#define EDITPANE_H

#include <QtGui>

class EditPane : public QLabel
{
    Q_OBJECT
public:
    explicit EditPane(QWidget *parent = 0, bool isLabel = true);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

    void setDimension(int width, int height) { this->width = width; this->height = height; }
    void setOffsets(int x, int y);
    void setCropOfsY(int ofs) { cropOfsY = ofs; }
    void setImage(QImage* image, int width, int height);
    void setExcluded(bool excluded) { this->excluded = excluded; }
    void setAllowSelection(bool value) { allowSelection = value; }
    void setIsLabel(bool value) { isLabel = value; }
    QVector<int> getSelection();
    void removeSelection() { if (allowSelection && validSelection) { validSelection = false; } }
    void setScreenRatio(double ratio) { screenRatioIn = ratio; cineBarFactor = (1.0 - (screenRatio / screenRatioIn)) / 2.0; }

signals:
    void onMouseClicked(QMouseEvent *event);
    void selectionPerformed(bool validSelection);

private:
    QImage* img = 0;
    QColor color1 = Qt::blue;
    QColor color2 = Qt::black;
    int width = 1920;
    int height = 1080;
    int ofsX = 0;
    int ofsY = 0;
    int imgWidth = 0;
    int imgHeight = 0;
    double screenRatioIn = 21.0 / 9;
    double cineBarFactor = 5.0 / 42;
    static constexpr double screenRatio = 16.0 / 9;
    int cropOfsY = 0;
    int inset = 2;
    bool validSelection = false;
    bool isLabel = true;
    int selectStartX = -1;
    int selectEndX = 0;
    int selectStartY = 0;
    int selectEndY = 0;
    bool allowSelection = false;
    bool leftButtonPressed = false;
    bool excluded = false;
    double xScaleCaption = 0;
    double yScaleCaption = 0;
    int yCrop = 0;

    void Initialize(bool isLayoutPane);
    
};

#endif // EDITPANE_H
