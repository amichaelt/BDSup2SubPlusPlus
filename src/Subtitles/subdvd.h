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

#ifndef SUBDVD_H
#define SUBDVD_H

#include "substream.h"
#include "substreamdvd.h"
#include <QObject>
#include <QString>
#include <QFile>
#include <QVector>

class SubtitleProcessor;
class SubPictureDVD;
class FileBuffer;

class SubDVD : public QObject, public Substream, public SubstreamDVD
{
    Q_OBJECT

public:
    SubDVD(QString subFileName, QString idxFileName, SubtitleProcessor* subtitleProcessor);

    Palette *getPalette() { return palette; }
    Bitmap *getBitmap() { return bitmap; }
    QImage *getImage();
    QImage *getImage(Bitmap *bitmap);
    int getPrimaryColorIndex() { return primaryColorIndex; }
    void decode(int index);
    int getNumFrames();
    int getNumForcedFrames() { return numForcedFrames; }
    bool isForced(int index);
    long getEndTime(int index);
    long getStartTime(int index);
    long getStartOffset(int index);
    SubPicture *getSubPicture(int index);
    QVector<int>& getFrameAlpha(int index);
    QVector<int>& getFramePal(int index);
    QVector<int>& getOriginalFrameAlpha(int index);
    QVector<int>& getOriginalFramePal(int index);
    Palette *getSrcPalette() { return srcPalette; }
    void setSrcPalette(Palette *palette) { srcPalette = palette; }
    int getLanguageIdx() { return languageIdx; }
    int getLanguageIdxRead() { return languageIdxRead; }

    Palette* decodePalette(SubPictureDVD* pic, Palette* palette);
    Bitmap* decodeImage(SubPictureDVD* pic, FileBuffer* fileBuffer, int transIdx);

    void readIdx(int idxToRead = -1);
    void writeIdx(QString filename, SubPicture* subPicture, QVector<int> offsets, QVector<int> timestamps, Palette* palette);
    void readSubFrame(SubPictureDVD* pic, long endOfs);
    void readAllSubFrames();
    QVector<uchar> createSubFrame(SubPictureDVD* subPicture, Bitmap* bitmap);

    void setTimeOffset(QString value) { timeOffset = value; }

signals:
    void maxProgressChanged(int maxProgress);
    void currentProgressChanged(int currentProgress);
    void addLanguage(const QString &message);

private:
    int streamID = 0;
    int languageIdxRead;
    QString idxFileName;
    QString subFileName;
    QString timeOffset = "";
    int delay = -1;

    QVector<SubPictureDVD*> subPictures;

    void decode(SubPictureDVD* pic);
    void decodeLine(QVector<uchar> src, int srcOfs, int srcLen, QImage *trg, int trgOfs, int width, int maxPixels);

    QVector<uchar> packHeader = {
        0x00, 0x00, 0x01, 0xba,                         // 0:  0x000001ba - packet ID
        0x44, 0x02, 0xc4, 0x82, 0x04, 0xa9,             // 4:  system clock reference
        0x01, 0x89, 0xc3,                               // 10: multiplexer rate
        0xf8,                                           // 13: stuffing info
    };

    QVector<uchar> headerFirst = {                      // header only in first packet
        0x00, 0x00, 0x01, 0xbd,                         // 0: 0x000001bd - sub ID
        0x00, 0x00,                                     // 4: packet length
        0x81, 0x80,                                     // 6:  packet type
        0x05,                                           // 8:  PTS length
        0x00, 0x0, 0x00, 0x00, 0x00,                    // 9:  PTS
        0x20,                                           // 14: stream ID
        0x00, 0x00,                                     // 15: Subpicture size in bytes
        0x00, 0x00,                                     // 17: offset to control header
    };

    QVector<uchar> headerNext = {                       // header in following packets
        0x00, 0x00, 0x01, 0xbd,                         // 0: 0x000001bd - sub ID
        0x00, 0x00,                                     // 4: packet length
        0x81, 0x00,                                     // 6: packet type
        0x00,                                           // 8: PTS length = 0
        0x20                                            // 9: Stream ID
    };

    QVector<uchar> controlHeader = {
        0x00,                                           //  dummy byte (for shifting when forced)
        0x00, 0x00,                                     //  0: offset to end sequence
        0x01,                                           //  2: CMD 1: start displaying
        0x03, 0x32, 0x10,                               //  3: CMD 3: Palette Info
        0x04, 0xff, 0xff,                               //  6: CMD 4: Alpha Info
        0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,       //  9: CMD 5: sub position
        0x06, 0x00, 0x00, 0x00, 0x00,                   // 16: CMD 6: rle offsets
        0xff,                                           // 21: End of control header
        0x00, 0x00,                                     // 22: display duration in 90kHz/1024
        0x00, 0x00,                                     // 24: offset to end sequence (again)
        0x02, 0xff,                                     // 26: CMD 2: stop displaying
    };
};

#endif // SUBDVD_H
