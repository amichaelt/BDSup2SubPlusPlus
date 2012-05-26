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

#ifndef SUPHD_H
#define SUPHD_H

#include "substream.h"
#include <QObject>
#include <QString>

class SubtitleProcessor;
class SubPictureHD;
class QImage;
class BitStream;

class SupHD : public QObject, public Substream
{
    Q_OBJECT

public:
    SupHD(QString fileName, SubtitleProcessor* subtitleProcessor);

    Palette *getPalette();
    Bitmap *getBitmap();
    QImage *getImage();
    QImage *getImage(Bitmap *bitmap);
    int getPrimaryColorIndex();
    void decode(int index);
    int getNumFrames();
    int getNumForcedFrames();
    bool isForced(int index);
    void close();
    long getEndTime(int index);
    long getStartTime(int index);
    long getStartOffset(int index);
    SubPicture *getSubPicture(int index);

    void readAllSupFrames();

signals:
    void maxProgressChanged(int maxProgress);
    void currentProgressChanged(int currentProgress);

private:
    QString supFileName;
    QVector<SubPictureHD*> subPictures = QVector<SubPictureHD*>();
    FileBuffer *fileBuffer = 0;
    Palette *palette = 0;
    Bitmap *bitmap = 0;
    int primaryColorIndex = 0;

    void decode(SubPictureHD* subPicture);
    void decodeLine(QImage* trg, int trgOfs, int width, int maxPixels, BitStream* src);
    Palette* decodePalette(SubPictureHD* subPicture);
    Bitmap* decodeImage(SubPictureHD* subPicture, int transparentIndex);

};

#endif // SUPHD_H
