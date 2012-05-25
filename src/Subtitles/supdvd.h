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

#ifndef SUPDVD_H
#define SUPDVD_H

#include "substream.h"
#include "substreamdvd.h"

#include <QObject>
#include <QString>
#include <QFile>

class SubtitleProcessor;

class SupDVD : public QObject, public Substream, public SubstreamDVD
{
    Q_OBJECT

public:
    SupDVD(QString supFileName, QString ifoFileName, SubtitleProcessor* subtitleProcessor);

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
    QVector<int> getFrameAlpha(int index);
    QVector<int> getFramePal(int index);
    QVector<int> getOriginalFrameAlpha(int index);
    QVector<int> getOriginalFramePal(int index);
    Palette *getSrcPalette();
    void setSrcPalette(Palette *palette);
    int getLanguageIdx();

    void readIfo();
    void readAllSupFrames();

signals:
    void maxProgressChanged(int maxProgress);
    void currentProgressChanged(int currentProgress);

private:
    QString supFileName;
    QString ifoFileName;

    QVector<SubPictureDVD*> subPictures = QVector<SubPictureDVD*>(0);

    const QVector<uchar> IFOheader = { 0x44, 0x56, 0x44, 0x56, 0x49, 0x44, 0x45, 0x4F, 0x2D, 0x56, 0x54, 0x53 };

    long readSupFrame(long ofs);
};

#endif // SUPDVD_H
