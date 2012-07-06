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

#ifndef ZOOMABLEIMAGEAREA_H
#define ZOOMABLEIMAGEAREA_H

#include <QtGui>

class ZoomableImageArea : public QLabel
{
    Q_OBJECT
public:
    explicit ZoomableImageArea(QWidget *parent = 0);
    void setImage(QImage* image);
    void updateImage();

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    int zoomScale = 0;
    double scaleFactor = 0.0;
    bool settingPixmap = false;
    QScopedPointer<QImage> image;
    QPixmap* drawPixmap = 0;
    QPainter* painter = new QPainter();
    QSize originalSize;

    void setZoomScale(int scale);
};

#endif // ZOOMABLEIMAGEAREA_H
