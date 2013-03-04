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

#ifndef SUPHD_H
#define SUPHD_H

#include "substream.h"
#include "bitmap.h"
#include "palette.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QScopedPointer>

class SubtitleProcessor;
class SubPictureHD;
class QImage;
class BitStream;

class SupHD : public QObject, public Substream
{
    Q_OBJECT

public:
    SupHD(QString fileName, SubtitleProcessor* subtitleProcessor);
    ~SupHD();

    void decode(int index);
    void readAllSupFrames();

    int primaryColorIndex() { return _primaryColorIndex; }
    int numFrames();
    int numForcedFrames() { return 0; }

    qint64 endTime(int index);
    qint64 startTime(int index);
    qint64 startOffset(int index);
    bool isForced(int index) { return false; }

    Bitmap &bitmap() { return _bitmap; }

    Palette &palette() { return _palette; }

    QImage image();
    QImage image(Bitmap &bitmap);

    SubPicture *subPicture(int index);

signals:
    void maxProgressChanged(qint64 maxProgress);
    void currentProgressChanged(qint64 currentProgress);

private:
    int _primaryColorIndex = 0;

    Bitmap _bitmap;

    Palette _palette;

    QScopedPointer<FileBuffer> fileBuffer;

    QString supFileName;

    QVector<SubPictureHD> subPictures;

    SubtitleProcessor* subtitleProcessor = 0;

    void decode(SubPictureHD &subPicture);
    void decodeLine(QImage &trg, int trgOfs, int width, int maxPixels, BitStream* src);

    Bitmap decodeImage(SubPictureHD &subPicture, int transparentIndex);

    Palette decodePalette(SubPictureHD &subPicture);
};

#endif // SUPHD_H
