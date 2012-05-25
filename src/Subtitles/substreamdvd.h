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

#ifndef SUBSTREAMDVD_H
#define SUBSTREAMDVD_H

#include <QVector>

class Bitmap;
class Palette;
class SubPictureDVD;
class FileBuffer;
class QImage;
class SubtitleProcessor;

class SubstreamDVD
{
public:
    SubstreamDVD();
    virtual QVector<int> getFrameAlpha(int index) = 0;
    virtual QVector<int> getFramePal(int index) = 0;
    virtual QVector<int> getOriginalFrameAlpha(int index) = 0;
    virtual QVector<int> getOriginalFramePal(int index) = 0;
    virtual Palette *getSrcPalette() = 0;
    virtual void setSrcPalette(Palette *palette) = 0;
    virtual int getLanguageIdx() = 0;
    void decode(SubPictureDVD* pic, SubtitleProcessor* subtitleProcessor);
    static Palette* decodePalette(SubPictureDVD *pic, Palette *palette, int alphaCrop);

protected:
    Palette *srcPalette;
    FileBuffer *fileBuffer = 0;
    Palette *palette = 0;
    Bitmap *bitmap = 0;

    QVector<int> lastAlpha = { 0, 0xf, 0xf, 0xf };

    int screenWidth = 720;
    int screenHeight = 576;
    int ofsXglob = 0;
    int ofsYglob = 0;
    int delayGlob = 0;
    int languageIdx = 0;
    int primaryColorIndex = 0;
    QVector<SubPictureDVD*> subPictures = QVector<SubPictureDVD*>(0);

private:
    Bitmap* decodeImage(SubPictureDVD* pic, Palette* palette, FileBuffer *fileBuffer, int transIdx);
    void decodeLine(QVector<uchar> src, int srcOfs, int srcLen, QImage* trg, int trgOfs, int width, int maxPixels);
};

#endif // SUBSTREAMDVD_H
