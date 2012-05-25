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

#include "subtitleprocessor.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QMessageBox>
#include <QTextStream>
#include "types.h"
#include "subdvd.h"
#include "supdvd.h"
#include "supbd.h"
#include "suphd.h"
#include "supxml.h"
#include "substream.h"
#include "subpicture.h"
#include "subpicturedvd.h"
#include "Filters/filter.h"
#include "Filters/filters.h"
#include "bitmap.h"
#include "palettebitmap.h"
#include "Tools/timeutil.h"
#include <cmath>

SubtitleProcessor::SubtitleProcessor()
{
    defaultDVDPalette = new Palette(defaultPalR, defaultPalG, defaultPalB, defaultAlpha, true);
    currentDVDPalette = new Palette(defaultPalR, defaultPalG, defaultPalB, defaultAlpha, true);
}

QByteArray SubtitleProcessor::getFileID(QString fileName, int numberOfBytes)
{
    QFile file(fileName);
    if (file.size() < numberOfBytes || !file.open(QIODevice::ReadOnly))
    {
        return QByteArray();
    }
    QByteArray buf = file.read(4);
    file.close();
    return buf;
}

StreamID SubtitleProcessor::getStreamID(QByteArray id)
{
    if (id[0] == (char)0x50 && id[1] == (char)0x47)
    {
        return StreamID::BDSUP;
    }
    else if (id[0] == (char)0x53 && id[1] == (char)0x50)
    {
        return StreamID::SUP;
    }
    else if (id[0] == (char)0x00 && id[1] == (char)0x00 && id[2] == (char)0x01 && id[3] == (char)0xba)
    {
        return StreamID::DVDSUB;
    }
    else if (id[0] == (char)0x23 && id[1] == (char)0x20 && id[2] == (char)0x56 && id[3] == (char)0x6f)
    {
        return StreamID::IDX;
    }
    else if (id[0] == (char)0x3c && id[1] == (char)0x3f && id[2] == (char)0x78 && id[3] == (char)0x6d)
    {
        return StreamID::XML;
    }
    else if (id[0] == (char)0x44 && id[1] == (char)0x56 && id[2] == (char)0x44 && id[3] == (char)0x56)
    {
        return StreamID::IFO;
    }
    return StreamID::UNKNOWN;
}

int SubtitleProcessor::getNumberOfFrames()
{
    if (substream == 0)
    {
        return 0;
    }
    return substream->getNumFrames();
}

QImage *SubtitleProcessor::getSrcImage()
{
    return substream->getImage();
}

QImage *SubtitleProcessor::getSrcImage(int index)
{
    substream->decode(index);
    return substream->getImage();
}

Resolution SubtitleProcessor::getResolution(int width, int height)
{
    if (width <= resolutions[0][0] && height <= resolutions[0][1])
    {
        return Resolution::NTSC;
    }
    if (width <= resolutions[1][0] && height <= resolutions[1][1])
    {
        return Resolution::PAL;
    }
    if (width <= resolutions[2][0] && height <= resolutions[2][1])
    {
        return Resolution::HD_720;
    }
    if (width <= resolutions[3][0] && height <= resolutions[3][1])
    {
        return Resolution::HD_1440x1080;
    }
    if (width <= resolutions[3][0] && height <= resolutions[3][1])
    {
        return Resolution::HD_1440x1080;
    }
    return Resolution::HD_1080;
}

SubPicture *SubtitleProcessor::getSubPictureSrc(int index)
{
    return substream->getSubPicture(index);
}

long SubtitleProcessor::syncTimePTS(long timeStamp, double fps)
{
    long retval;
    // correct time stamps to fit to frames
    if (fps == FPS_NTSC || fps == FPS_PAL || fps == FPS_24HZ)
    {
        // NTSC: 90000/(30000/1001) = 3003
        // PAL:  90000/25 = 3600
        // 24Hz: 90000/24 = 3750
        int tpfi = (int)((90000 + (fps / 2)) /fps); // target time per frame in 90kHz
        int tpfh = tpfi / 2;
        retval = ((timeStamp + tpfh) / tpfi) * tpfi;
    }
    else if (fpsTrg == FPS_24P)
    {
        // 90000/(24000/1001) = 3753.75 = 15015/4
        retval = ((((timeStamp + 1877) * 4) / 15015) * 15015) / 4;
    }
    else
    {
        double tpf = (90000 / fpsTrg); // target time per frame in 90kHz
        retval = (long)(((long)(timeStamp / tpf) * tpf) + 0.5);
    }
    return retval;
}

void SubtitleProcessor::close()
{
    //TODO: fix when other classes are implemented
    if (supBD != 0)
    {
        supBD->close();
    }
    if (supHD != 0)
    {
        supHD->close();
    }
    if (supXML != 0)
    {
        supXML->close();
    }
    if (subDVD != 0)
    {
        subDVD->close();
    }
    if (supDVD != 0)
    {
        supDVD->close();
    }
}

void SubtitleProcessor::scanSubtitles()
{
    subPictures = QVector<SubPicture*>(substream->getNumFrames());
    SubPicture* picSrc;

    double factTS = 1.0;

    if (convertFPS)
    {
        factTS = fpsSrc / fpsTrg;
    }
    else
    {
        factTS = 1.0;
    }

    // change target resolution to source resolution if no conversion is needed
    if (!convertResolution && getNumberOfFrames() > 0)
    {
        resolutionTrg = getResolution(getSubPictureSrc(0)->width, getSubPictureSrc(0)->height);
    }

    double fx;
    double fy;
    if (applyFreeScale)
    {
        fx = freeScaleX;
        fy = freeScaleY;
    }
    else
    {
        fx = 1.0;
        fy = 1.0;
    }

    // first run: clone source subpics, apply speedup/down,
    for (int i = 0; i < subPictures.size(); ++i)
    {
        picSrc = substream->getSubPicture(i);
        subPictures.replace(i, picSrc->copy());
        long ts = picSrc->startTime;
        long te = picSrc->endTime;
        // copy time stamps and apply speedup/speeddown
        if (!convertFPS)
        {
            subPictures.at(i)->startTime = ts + delayPTS;
            subPictures.at(i)->endTime = te + delayPTS;
        }
        else
        {
            subPictures.at(i)->startTime= (long)((ts * factTS) + 0.5) + delayPTS;
            subPictures.at(i)->endTime = (long)((te * factTS) + 0.5) + delayPTS;
        }
        // synchronize to target frame rate
        subPictures.at(i)->startTime = syncTimePTS(subPictures.at(i)->startTime, fpsTrg);
        subPictures.at(i)->endTime = syncTimePTS(subPictures.at(i)->endTime, fpsTrg);

        // set forced flag
        SubPicture* picTrg = subPictures.at(i);
        switch ((int)forceAll)
        {
            case (int)SetState::SET:
                picTrg->isForced = true;
                break;
            case (int)SetState::CLEAR:
                picTrg->isForced = false;
                break;
        }

        double scaleX;
        double scaleY;
        if (convertResolution)
        {
            // adjust image sizes and offsets
            // determine scaling factors
            picTrg->width = getResolution(resolutionTrg)[0];
            picTrg->height = getResolution(resolutionTrg)[1];
            scaleX = (double)picTrg->width / picSrc->width;
            scaleY = (double)picTrg->height / picSrc->height;
        }
        else
        {
            picTrg->width = picSrc->width;
            picTrg->height = picSrc->height;
            scaleX = 1.0;
            scaleY = 1.0;
        }

        int width = (int)(((picSrc->getImageWidth() * scaleX) * fx) + 0.5);
        if (width < minDim)
        {
            width = picSrc->getImageWidth();
        }
        else if (width > picTrg->width)
        {
            width = picTrg->width;
        }

        int height = (int)(((picSrc->getImageHeight() * scaleY) * fy) + 0.5);
        if (height < minDim)
        {
            height = picSrc->getImageHeight();
        }
        else if (height > picTrg->height)
        {
            height = picTrg->height;
        }

        picTrg->setImageWidth(width);
        picTrg->setImageHeight(height);

        int xOfs = (int)((picSrc->getOfsX() * scaleX) + 0.5);
        int spaceSrc = (int)(((picSrc->width - picSrc->getImageWidth()) * scaleX) + 0.5);
        int spaceTrg = picTrg->width - width;
        xOfs += ((spaceTrg - spaceSrc) / 2);
        if (xOfs < 0)
        {
            xOfs = 0;
        }
        else if (xOfs+width > picTrg->width)
        {
            xOfs = picTrg->width - width;
        }
        picTrg->setOfsX(xOfs);

        int yOfs = (int)(picSrc->getOfsY() * scaleY + 0.5);
        spaceSrc = (int)((picSrc->height-picSrc->getImageHeight())*scaleY + 0.5);
        spaceTrg = picTrg->height - height;
        yOfs += ((spaceTrg - spaceSrc) / 2);
        if (yOfs+height > picTrg->height)
        {
            yOfs = picTrg->height - height;
        }
        picTrg->setOfsY(yOfs);
    }

    // 2nd run: validate times
    SubPicture* picPrev = 0;
    SubPicture* picNext;
    for (int i = 0; i < subPictures.size(); ++i)
    {
        if (i < subPictures.size() - 1)
        {
            picNext = subPictures.at(i + 1);
        }
        else
        {
            picNext = 0;
        }
        picSrc = subPictures.at(i);
        validateTimes(i, subPictures.at(i), picNext, picPrev);
        picPrev = picSrc;
    }
}

QVector<int> SubtitleProcessor::getResolution(Resolution resolution)
{
    return resolutions.at((int) resolution);
}

void SubtitleProcessor::validateTimes(int index, SubPicture *subPicture, SubPicture *subPictureNext, SubPicture *subPicturePrevious)
{
    long ts =  subPicture->startTime;   // start time
    long te =  subPicture->endTime;     // end time
    long delay = 5000 * 90;             // default delay for missing end time (5 seconds)

    index += 1; // only used for display

    // get end time of last frame
    long te_last;
    if (subPicturePrevious != 0)
    {
        te_last = subPicturePrevious->endTime;
    }
    else
    {
        te_last = -1;
    }

    if (ts < te_last)
    {
        //TODO: print warning
        ts = te_last;
    }

    // get start time of next frame
    long ts_next;
    if (subPictureNext != 0)
    {
        ts_next = subPictureNext->startTime;
    }
    else
    {
        ts_next = 0;
    }

    if (ts_next == 0) {
        if ( te > ts)
            ts_next = te;
        else {
            // completely fucked:
            // end time and next start time are invalid
            ts_next = ts+delay;
        }
    }

    if (te <= ts)
    {
        if (te == 0)
        {
            //TODO: print warning
        }
        else
        {
            //TODO: print warning
        }
        te = ts + delay;
        if (te > ts_next)
        {
            te = ts_next;
        }
    }
    else if (te > ts_next)
    {
        //TODO: print warning
        te = ts_next;
    }

    if ((te - ts) < minTimePTS)
    {
        if (fixShortFrames)
        {
            te = ts + minTimePTS;
            if (te > ts_next)
            {
                te = ts_next;
            }
            //TODO: print warning
        }
        else
        {
            //TODO: print warning
        }
    }

    if (subPicture->startTime != ts)
    {
        subPicture->startTime = syncTimePTS(ts, fpsTrg);
    }
    if (subPicture->endTime != te)
    {
        subPicture->endTime = syncTimePTS(te, fpsTrg);
    }
}

void SubtitleProcessor::readSubtitleStream()
{
    emit windowTitleChanged(progNameVer + " - " + QDir::toNativeSeparators(fileName));

    QFileInfo fileInfo(fileName);
    QString extension = fileInfo.suffix().toLower();
    bool isXML(extension == "xml"), isIDX(extension == "idx"), isIFO(extension == "ifo");

    QByteArray id = getFileID(fileName, 4);
    StreamID streamID = (id.isEmpty()) ? StreamID::UNKNOWN : getStreamID(id);

    if (!isXML && !isIDX && !isIFO && streamID == StreamID::UNKNOWN)
    {
        //TODO: add error handling
        return;
    }

    maxProgress = fileInfo.size();
    lastProgress = 0;


    if (isXML || streamID == StreamID::XML)
    {
        readXml();
    }
    else if (isIDX || streamID == StreamID::DVDSUB || streamID == StreamID::IDX)
    {
        readDVDSubStream(streamID, true);
    }
    else if (isIFO || streamID == StreamID::IFO)
    {
        readDVDSubStream(streamID, false);
    }
    else
    {
        if (QFile(fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".ifo").exists())
        {
            readDVDSubStream(streamID, false);
        }
        else
        {
            readSup();
        }
    }

    emit progressDialogVisibilityChanged(false);
    emit loadingSubtitleFinished();
}

void SubtitleProcessor::moveAll()
{
    maxProgress = substream->getNumFrames();
    lastProgress = 0;

    emit progressDialogTitleChanged("Moving");
    emit progressDialogTextChanged("Moving all captions");
    emit progressDialogVisibilityChanged(true);

    moveAllToBounds();

    emit progressDialogVisibilityChanged(false);
    emit moveAllFinished();
}

void SubtitleProcessor::cancelLoading()
{
    isActive = false;
}

void SubtitleProcessor::moveAllToBounds()
{
    QString sy("");
    switch ((int)moveModeY)
    {
        case (int)MoveModeY::INSIDE:
            sy = "inside";
            break;
        case (int)MoveModeY::OUTSIDE:
            sy = "outside";
            break;
    }

    QString sx("");
    switch ((int)moveModeX)
    {
        case (int)MoveModeX::CENTER:
            sx = "center vertically";
            break;
        case (int)MoveModeX::LEFT:
            sx = "left";
            break;
        case (int)MoveModeX::RIGHT:
            sx = "right";
    }

    QString s("Moving captions ");
    if (!sy.isEmpty())
    {
        s += sy + " cinemascope bars";
        if (!sx.isEmpty())
        {
            s += " and to the " + sx;
        }
        //TODO: add printing
    }
    else if (!sx.isEmpty())
    {
        //TODO: add printing
    }
    if (!cliMode)
    {
        // in CLI mode, moving is done during export
        for (int idx = 0; idx < subPictures.size(); ++idx)
        {
            setCurrentProgress(idx);
            if (!subPictures.at(idx)->wasDecoded)
            {
                convertSup(idx, idx + 1, subPictures.size(), true);
            }
            moveToBounds(subPictures.at(idx), idx + 1, cineBarFactor, moveOffsetX, moveOffsetY, moveModeX, moveModeY, cropOfsY);
        }
    }
}

void SubtitleProcessor::convertSup(int index, int displayNumber, int displayMax)
{
    convertSup(index, displayNumber, displayMax, false);
}

void SubtitleProcessor::convertSup(int index, int displayNumber, int displayMax, bool skipScaling)
{
    int width, height;
    int startOfs = (int)substream->getStartOffset(index);
    SubPicture* subPic = substream->getSubPicture(index);

    //TODO: print decoding message

    substream->decode(index);
    width = subPic->getImageWidth();
    height = subPic->getImageHeight();
    if (outMode == OutputMode::VOBSUB || outMode == OutputMode::SUPIFO)
    {
        determineFramePalette(index);
    }
    updateTrgPic(index);
    SubPicture* picTrg = subPictures.at(index);
    picTrg->wasDecoded = true;

    int trgWidth = picTrg->getImageWidth();
    int trgHeight = picTrg->getImageHeight();
    if (trgWidth < minDim || trgHeight < minDim || width < minDim || height < minDim)
    {
        // don't scale to avoid division by zero in scaling routines
        trgWidth = width;
        trgHeight = height;
    }

    if (!skipScaling)
    {
        Filter* scaleFilter = Filters::getFilter(scalingFilter);

        Bitmap* targetBitmap;
        Palette* targetPalette = trgPal;
        // create scaled bitmap
        if (outMode == OutputMode::VOBSUB || outMode == OutputMode::SUPIFO)
        {
            // export 4 color palette
            if (width == trgWidth && height == trgHeight)
            {
                // don't scale at all
                if ((inMode == InputMode::VOBSUB || inMode == InputMode::SUPIFO) && paletteMode == PaletteMode::KEEP_EXISTING)
                {
                    targetBitmap = substream->getBitmap(); // no conversion
                }
                else
                {
                    targetBitmap = substream->getBitmap()->convertLm(substream->getPalette(), alphaThreshold, luminanceThreshold); // reduce palette
                }
            }
            else
            {
                // scale up/down
                if ((inMode == InputMode::VOBSUB || inMode == InputMode::SUPIFO) && paletteMode == PaletteMode::KEEP_EXISTING)
                {
                    // keep palette
                    if (scaleFilter != 0)
                    {
                        targetBitmap = substream->getBitmap()->scaleFilter(trgWidth, trgHeight, substream->getPalette(), scaleFilter);
                    }
                    else
                    {
                        targetBitmap = substream->getBitmap()->scaleBilinear(trgWidth, trgHeight, substream->getPalette());
                    }
                }
                else
                {
                    // reduce palette
                    if (scaleFilter != 0)
                    {
                        targetBitmap = substream->getBitmap()->scaleFilterLm(trgWidth, trgHeight, substream->getPalette(), alphaThreshold, luminanceThreshold, scaleFilter);
                    }
                    else
                    {
                        targetBitmap = substream->getBitmap()->scaleBilinearLm(trgWidth, trgHeight, substream->getPalette(), alphaThreshold, luminanceThreshold);
                    }
                }
            }
        }
        else
        {
            // export (up to) 256 color palette
            targetPalette = substream->getPalette();
            if (width == trgWidth && height == trgHeight)
            {
                targetBitmap = substream->getBitmap(); // no scaling, no conversion
            }
            else
            {
                // scale up/down
                if (paletteMode == PaletteMode::KEEP_EXISTING)
                {
                    // keep palette
                    if (scaleFilter != 0)
                    {
                        targetBitmap = substream->getBitmap()->scaleFilter(trgWidth, trgHeight, substream->getPalette(), scaleFilter);
                    }
                    else
                    {
                        targetBitmap = substream->getBitmap()->scaleBilinear(trgWidth, trgHeight, substream->getPalette());
                    }
                }
                else
                {
                    // create new palette
                    bool dither = paletteMode == PaletteMode::CREATE_DITHERED;
                    PaletteBitmap* paletteBitmap;
                    if (scaleFilter != 0)
                    {
                        paletteBitmap = substream->getBitmap()->scaleFilter(trgWidth, trgHeight, substream->getPalette(), scaleFilter, dither);
                    }
                    else
                    {
                        paletteBitmap = substream->getBitmap()->scaleBilinear(trgWidth, trgHeight, substream->getPalette(), dither);
                    }
                    targetBitmap = paletteBitmap->bitmap;
                    targetPalette = paletteBitmap->palette;
                }
            }
        }
        if (!picTrg->erasePatch.isEmpty())
        {
            trgBitmapUnpatched = new Bitmap(targetBitmap);
            int col = targetPalette->getTransparentIndex();
            for (auto erasePatch : picTrg->erasePatch)
            {
                targetBitmap->fillRect(erasePatch->x, erasePatch->y, erasePatch->w, erasePatch->h, col);
            }
        }
        else
        {
            trgBitmapUnpatched = targetBitmap;
        }
        trgBitmap = targetBitmap;
        trgPal = targetPalette;
    }

    if (cliMode)
    {
        moveToBounds(picTrg, displayNumber, cineBarFactor, moveOffsetX, moveOffsetY, moveModeX, moveModeY, cropOfsY);
    }
}

void SubtitleProcessor::determineFramePalette(int index)
{
    if ((inMode != InputMode::VOBSUB && inMode != InputMode::SUPIFO) || paletteMode != PaletteMode::KEEP_EXISTING)
    {
        // get the primary color from the source palette
        QVector<int> rgbSrc(substream->getPalette()->getRGB(substream->getPrimaryColorIndex()));

        // match with primary color from 16 color target palette
        // note: skip index 0 , primary colors at even positions
        // special treatment for index 1:  white
        Palette* trgPalette = currentDVDPalette;
        int minDistance = 0xffffff; // init > 0xff*0xff*3 = 0x02fa03
        int colIdx = 0;
        for (int idx = 1; idx < trgPalette->getSize(); idx += 2 )
        {
            QVector<int> rgb(trgPalette->getRGB(idx));
            // distance vector (skip sqrt)
            int rd = rgbSrc[0] - rgb[0];
            int gd = rgbSrc[1] - rgb[1];
            int bd = rgbSrc[2] - rgb[2];
            int distance = (rd * rd) + (gd * gd) + (bd * bd);
            // new minimum distance ?
            if (distance < minDistance)
            {
                colIdx = idx;
                minDistance = distance;
                if (minDistance == 0)
                {
                    break;
                }
            }
            // special treatment for index 1 (white)
            if (idx == 1)
            {
                idx--; // -> continue with index = 2
            }
        }

        // set new frame palette
        QVector<int> paletteFrame(4);
        paletteFrame.replace(0, 0);        // black - transparent color
        paletteFrame.replace(1, colIdx);   // primary color
        if (colIdx == 1)
        {
            paletteFrame.replace(2, colIdx + 2); // special handling: white + dark grey
        }
        else
        {
            paletteFrame.replace(2, colIdx + 1); // darker version of primary color
        }
        paletteFrame.replace(3, 0);        // black - opaque

        subVobTrg->alpha = alphaDefault;
        subVobTrg->pal = paletteFrame;

        trgPal = SubstreamDVD::decodePalette(subVobTrg, trgPalette, alphaCrop);
    }
    else
    {
        SubstreamDVD* substreamDVD;
        // use palette from loaded VobSub or SUP/IFO
        Palette* miniPal = new Palette(4, true);
        QVector<int> alpha;
        QVector<int> paletteFrame;

        if (inMode == InputMode::VOBSUB)
        {
            substreamDVD = subDVD;
        }
        else
        {
            substreamDVD = supDVD;
        }

        alpha = substreamDVD->getFrameAlpha(index);
        paletteFrame = substreamDVD->getFramePal(index);

        for (int i = 0; i < 4; ++i)
        {
            int a = (alpha[i] * 0xff) / 0xf;
            if (a >= alphaCrop)
            {
                miniPal->setARGB(i, currentSourceDVDPalette->getARGB(paletteFrame[i]));
                miniPal->setAlpha(i, a);
            }
            else
            {
                miniPal->setARGB(i, 0);
            }
        }
        subVobTrg->alpha = alpha;
        subVobTrg->pal = paletteFrame;
        trgPal = miniPal;
    }
}

bool SubtitleProcessor::updateTrgPic(int index)
{
    SubPicture* picSrc = substream->getSubPicture(index);
    SubPicture* picTrg = subPictures.at(index);
    double scaleX = (double)picTrg->width / picSrc->width;
    double scaleY = (double)picTrg->height / picSrc->height;
    double fx;
    double fy;
    if (applyFreeScale)
    {
        fx = freeScaleX;
        fy = freeScaleY;
    }
    else
    {
        fx = 1.0;
        fy = 1.0;
    }

    int wOld = picTrg->getImageWidth();
    int hOld = picTrg->getImageHeight();
    int wNew = (int)(((picSrc->getImageWidth()  * scaleX) * fx) + 0.5);
    if (wNew < minDim)
    {
        wNew = picSrc->getImageWidth();
    }
    else if (wNew > picTrg->width)
    {
        wNew = picTrg->width;
    }
    int hNew = (int)(((picSrc->getImageHeight() * scaleY) * fy) + 0.5);
    if (hNew < minDim)
    {
        hNew = picSrc->getImageHeight();
    }
    else if (hNew > picTrg->height)
    {
        hNew = picTrg->height;
    }
    picTrg->setImageWidth(wNew);
    picTrg->setImageHeight(hNew);
    if (wNew != wOld)
    {
        int xOfs = (int)((picSrc->getOfsX() * scaleX) + 0.5);
        int spaceSrc = (int)(((picSrc->width - picSrc->getImageWidth()) * scaleX) + 0.5);
        int spaceTrg = picTrg->width - wNew;
        xOfs += (spaceTrg - spaceSrc) / 2;
        if (xOfs < 0)
        {
            xOfs = 0;
        }
        else if (xOfs + wNew > picTrg->width)
        {
            xOfs = picTrg->width - wNew;
        }
        picTrg->setOfsX(xOfs);
    }
    if (hNew != hOld)
    {
        int yOfs = (int)((picSrc->getOfsY() * scaleY) + 0.5);
        int spaceSrc = (int)(((picSrc->height - picSrc->getImageHeight()) * scaleY) + 0.5);
        int spaceTrg = picTrg->height - hNew;
        yOfs += (spaceTrg - spaceSrc) / 2;
        if (yOfs + hNew > picTrg->height)
        {
            yOfs = picTrg->height - hNew;
        }
        picTrg->setOfsY(yOfs);
    }
    // was image cropped?
    return (wNew != wOld) || (hNew != hOld);
}

void SubtitleProcessor::addRecent(QString fileName)
{
    if (recentFiles.indexOf(fileName) == -1)
    {
        if (recentFiles.size() > 0 && recentFiles.size() < 5)
        {
            recentFiles.replaceInStrings(recentFiles.at(0), fileName);
        }
        else
        {
            recentFiles << fileName;
        }
    }
}

void SubtitleProcessor::moveToBounds(SubPicture *picture, int index, double barFactor, int offsetX, int offsetY, MoveModeX moveModeX, MoveModeY moveModeY, int cropOffsetY)
{
    //TODO: finish implementing
    throw 10;
}

QString SubtitleProcessor::getSrcInfoStr(int index)
{
    SubPicture* pic = substream->getSubPicture(index);
    QString text = "screen size: " + QString::number(pic->width) + "x" + QString::number(pic->height) + "    ";
    text +=	"image size: " + QString::number(pic->getImageWidth()) + "x" + QString::number(pic->getImageHeight()) + "    ";
    text += "pos: (" + QString::number(pic->getOfsX()) + "," + QString::number(pic->getOfsY()) + ") - (" + QString::number((pic->getOfsX() + pic->getImageWidth()))
          + "," + QString::number((pic->getOfsY() + pic->getImageHeight())) + ")    ";
    text += "start: " + TimeUtil::ptsToTimeStr(pic->startTime) + "    ";
    text += "end: " + TimeUtil::ptsToTimeStr(pic->endTime) + "    ";
    text += "forced: " + ((pic->isForced) ? QString("yes") : QString("no"));
    return text;
}

QString SubtitleProcessor::getTrgInfoStr(int index)
{
    SubPicture* pic = subPictures[index];
    QString text = "screen size: " + QString::number(getTrgWidth(index)) + "x" + QString::number(getTrgHeight(index)) + "    ";
    text +=	"image size: " + QString::number(getTrgImgWidth(index)) + "x" + QString::number(getTrgImgHeight(index)) + "    ";
    text += "pos: (" + QString::number(pic->getOfsX()) + "," + QString::number(pic->getOfsY()) + ") - (" + QString::number((pic->getOfsX() + getTrgImgWidth(index)))
          + "," + QString::number((pic->getOfsY() + getTrgImgHeight(index))) + ")    ";
    text += "start: " + TimeUtil::ptsToTimeStr(pic->startTime) + "    ";
    text += "end: " + TimeUtil::ptsToTimeStr(pic->endTime) + "    ";
    text += "forced: " + ((pic->isForced) ? QString("yes") : QString("no"));
    return text;
}

int SubtitleProcessor::getTrgWidth(int index)
{
    return subPictures.at(index)->width;
}

int SubtitleProcessor::getTrgHeight(int index)
{
    return subPictures.at(index)->height;
}

int SubtitleProcessor::getTrgOfsX(int index)
{
    return subPictures.at(index)->getOfsX();
}

int SubtitleProcessor::getTrgOfsY(int index)
{
    return subPictures.at(index)->getOfsY();
}

QImage *SubtitleProcessor::getTrgImage()
{
    return trgBitmap->getImage(trgPal);
}

int SubtitleProcessor::getTrgImgWidth(int index)
{
    return subPictures.at(index)->getImageWidth();
}

int SubtitleProcessor::getTrgImgHeight(int index)
{
    return subPictures.at(index)->getImageHeight();
}

bool SubtitleProcessor::getTrgExcluded(int index)
{
    return subPictures.at(index)->exclude;
}

void SubtitleProcessor::setFPSTrg(double trg)
{
    fpsTrg = trg;
    delayPTS = (int)syncTimePTS(delayPTS, trg);
    minTimePTS = (int)syncTimePTS(minTimePTS, trg);
    if (props == 0)
    {
        fpsTrgSet = true;
    }
}

double SubtitleProcessor::getFPS(QString string)
{
    string = string.trimmed().toLower();
    if (string == "pal"  || string == "25p" || string == "25")
    {
        return FPS_PAL;
    }
    if (string == "ntsc" || string == "30p" || string == "29.97" || string == "29.970")
    {
        return FPS_NTSC;
    }
    if (string == "24p"  || string == "23.976")
    {
        return FPS_24P;
    }
    if (string == "23.975")
    {
        return FPS_23_975;
    }
    if (string == "24")
    {
        return FPS_24HZ;
    }
    if (string == "50i"  || string == "50")
    {
        return FPS_PAL_I;
    }
    if (string == "60i"  || string == "59.94")
    {
        return FPS_NTSC_I;
    }

    bool ok;
    double d = string.toDouble(&ok);
    if (!ok)
    {
        return -1.0;
    }

    if (fabs(d - FPS_23_975) < 0.001)
    {
        return FPS_23_975;
    }
    if (fabs(d - FPS_24P) < 0.001)
    {
        return FPS_24P;
    }
    if (fabs(d - FPS_24HZ) < 0.001)
    {
        return FPS_24P;
    }
    if (fabs(d - FPS_PAL) < 0.001)
    {
        return FPS_PAL;
    }
    if (fabs(d - FPS_NTSC) < 0.001)
    {
        return FPS_NTSC;
    }
    if (fabs(d - FPS_NTSC_I) < 0.001)
    {
        return FPS_NTSC_I;
    }
    if (fabs(d - FPS_PAL_I) < 0.001)
    {
        return FPS_PAL_I;
    }
    return d;
}

void SubtitleProcessor::setMaxProgress(int maxProgress)
{
    this->maxProgress = maxProgress;
}

void SubtitleProcessor::setCurrentProgress(int currentProgress)
{
    int value = (int)(((long)currentProgress * 100) / maxProgress);
    if (value > lastProgress)
    {
        lastProgress = value;
        emit progressDialogValueChanged(value);
    }
}

void SubtitleProcessor::readXml()
{
    //TODO: print message
    numberOfErrors = 0;
    numberOfWarnings = 0;

    if (substream != 0)
    {
        substream->close();
    }

    supXML = new SupXML(fileName, this);
    connect(supXML, SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
    connect(supXML, SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
    supXML->readAllImages();
    substream = supXML;

    inMode = InputMode::XML;

    // decode first frame
    substream->decode(0);
    subVobTrg = new SubPictureDVD();

    // automatically set luminance thresholds for VobSub conversion
    int maxLum = substream->getPalette()->getY()[substream->getPrimaryColorIndex()] & 0xff;
    if (maxLum > 30)
    {
        luminanceThreshold.replace(0, (maxLum * 2) / 3);
        luminanceThreshold.replace(1, maxLum / 3);
    }
    else
    {
        luminanceThreshold.replace(0, 210);
        luminanceThreshold.replace(1, 160);
    }

    // find language idx
    for (int i = 0; i < languages.size(); ++i)
    {
        if (QString::compare(languages[i][2], supXML->getLanguage(), Qt::CaseInsensitive))
        {
            languageIdx = i;
            break;
        }
    }

    // set frame rate
    if (!fpsSrcSet)      // CLI override
    {
        fpsSrc = supXML->getFps();
        fpsSrcCertain = true;
        if (keepFps)
        {
            setFPSTrg(fpsSrc);
        }
    }
}

void SubtitleProcessor::readDVDSubStream(StreamID streamID, bool isVobSub)
{
    //TODO: print message
    numberOfErrors = 0;
    numberOfWarnings = 0;

    if (substream != 0)
    {
        substream->close();
    }

    SubstreamDVD* substreamDVD;
    QFileInfo fileInfo(fileName);

    if (isVobSub)
    {
        QString idxFileName;
        QString subFileName;
        if (streamID == StreamID::DVDSUB)
        {
            subFileName = fileName;
            idxFileName = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".idx";
        }
        else
        {
            idxFileName = fileName;
            subFileName = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".sub";
        }
        subDVD = new SubDVD(subFileName, idxFileName, this);
        connect(subDVD, SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(subDVD, SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        subDVD->readIdx();
        subDVD->readAllSubFrames();
        substream = subDVD;
        inMode = InputMode::VOBSUB;
        substreamDVD = subDVD;
    }
    else
    {
        QString ifoFileName;
        QString supFileName;
        if (fileInfo.suffix() == "sup")
        {
            ifoFileName = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".ifo";
            supFileName = fileName;
        }
        else
        {
            ifoFileName = fileName;
            supFileName = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".sup";
        }
        supDVD = new SupDVD(supFileName, ifoFileName, this);
        connect(supDVD, SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(supDVD, SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        supDVD->readIfo();
        supDVD->readAllSupFrames();
        substream = supDVD;
        inMode = InputMode::SUPIFO;
        substreamDVD = supDVD;
    }

    substream->decode(0);
    subVobTrg = new SubPictureDVD;
    defaultSourceDVDPalette = substreamDVD->getSrcPalette();
    currentSourceDVDPalette = new Palette(*defaultSourceDVDPalette);

    int primaryColorIndex = substream->getPrimaryColorIndex();
    int yMax = substream->getPalette()->getY()[primaryColorIndex] & 0xff;

    if (yMax > 10)
    {
        int yMin = yMax;
        Palette* palette = substream->getPalette();
        for (int i = 0; i < 4; ++i)
        {
            int y = palette->getY()[i] & 0xff;
            int a = palette->getAlpha(i);
            if (y < yMin && a > alphaThreshold)
            {
                yMin = y;
            }
        }

        int value = yMin + (((yMax - yMin) * 9) / 10);
        luminanceThreshold.replace(0, value);
        value = yMin + (((yMax - yMin) * 3) / 10);
        luminanceThreshold.replace(1, value);
    }
    else
    {
        luminanceThreshold.replace(0, 210);
        luminanceThreshold.replace(1, 160);
    }

    languageIdx = substreamDVD->getLanguageIdx();

    if (!fpsSrcSet)
    {
        int height = substream->getSubPicture(0)->height;

        switch(height)
        {
        case 480:
            fpsSrc = FPS_NTSC;
            useBT601 = true;
            fpsSrcCertain = true;
            break;
        case 576:
            fpsSrc = FPS_PAL;
            useBT601 = true;
            fpsSrcCertain = true;
            break;
        default:
            fpsSrc = FPS_24P;
            useBT601 = false;
            fpsSrcCertain = false;
            break;
        }
    }
}

void SubtitleProcessor::readSup()
{
    //TODO: print message
    numberOfErrors = 0;
    numberOfWarnings = 0;

    for (auto languageTriple : languages)
    {
        if (fileName.contains(languageTriple[0], Qt::CaseInsensitive))
        {
            languageIdx = languages.indexOf(languageTriple);
            //TODO: print language found message
            break;
        }
    }

    if (substream != 0)
    {
        substream->close();
    }

    QByteArray id = getFileID(fileName, 2);

    if (!id.isEmpty() && ((uchar)id[0] == 0x50 && (uchar)id[1] == 0x47))
    {
        supBD = new SupBD(fileName, this);
        connect(supBD, SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(supBD, SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        supBD->readAllSupFrames();
        substream = supBD;
        supHD = 0;
        inMode = InputMode::BDSUP;
    }
    else
    {
        supHD = new SupHD(fileName, this);
        connect(supHD, SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(supHD, SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        supHD->readAllSupFrames();
        substream = supHD;
        supBD = 0;
        inMode = InputMode::HDDVDSUP;
    }

    // decode first frame
    substream->decode(0);
    subVobTrg = new SubPictureDVD;

    // automatically set luminance thresholds for VobSub conversion
    int maxLum = substream->getPalette()->getY()[substream->getPrimaryColorIndex()] & 0xff;
    if (maxLum > 30)
    {
        luminanceThreshold.replace(0, (maxLum * 2) / 3);
        luminanceThreshold.replace(1, maxLum / 3);
    }
    else
    {
        luminanceThreshold.replace(0, 210);
        luminanceThreshold.replace(1, 160);
    }

    // try to detect source frame rate
    if (!fpsSrcSet) // CLI override
    {
        if (substream == supBD)
        {
            fpsSrc = supBD->getFps(0);
            fpsSrcCertain = true;
            if (keepFps)
            {
                setFPSTrg(fpsSrc);
            }
        }
        else
        {
            // for HD-DVD we need to guess
            useBT601 = false;
            fpsSrcCertain = false;
            fpsSrc = FPS_24P;
        }
    }
}

Resolution SubtitleProcessor::getResolution(QString string)
{
    if (string == "480i")
    {
        return Resolution::NTSC;
    }
    if (string == "576i")
    {
        return Resolution::PAL;
    }
    if (string == "720p")
    {
        return Resolution::HD_720;
    }
    if (string == "1440x1080")
    {
        return Resolution::HD_1440x1080;
    }
    return Resolution::HD_1080;
}

QVector<int> SubtitleProcessor::getResolutions(Resolution resolution)
{
    if (resolution == Resolution::NTSC)
    {
        return resolutions[0];
    }
    if (resolution == Resolution::PAL)
    {
        return resolutions[1];
    }
    if (resolution == Resolution::HD_720)
    {
        return resolutions[2];
    }
    if (resolution == Resolution::HD_1440x1080)
    {
        return resolutions[3];
    }
    if (resolution == Resolution::HD_1080)
    {
        return resolutions[4];
    }
}
