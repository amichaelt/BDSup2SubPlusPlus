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

#include "subtitleprocessor.h"
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
#include "palette.h"
#include "types.h"
#include "Filters/filters.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QTextStream>
#include <cmath>

SubtitleProcessor::SubtitleProcessor(QWidget* parent, QSettings* settings, bool loadSettings) :
    defaultDVDPalette(defaultPalR, defaultPalG, defaultPalB, defaultAlpha, true),
    currentDVDPalette(defaultPalR, defaultPalG, defaultPalB, defaultAlpha, true),
    fpsSrc(FPS_24P),
    fpsTrg(FPS_PAL),
    paletteMode(PaletteMode::KEEP_EXISTING),
    outMode(OutputMode::VOBSUB),
    inMode(InputMode::VOBSUB),
    resolutionTrg(Resolution::PAL),
    forceAll(SetState::KEEP),
    moveModeY(MoveModeY::KEEP),
    moveModeX(MoveModeX::KEEP),
    scalingFilter(ScalingFilters::BILINEAR)
{
    this->parent = parent;
    this->settings = settings;

    if (settings != 0 && loadSettings)
    {
        SetValuesFromSettings();
    }
}

SubtitleProcessor::~SubtitleProcessor()
{
}

void SubtitleProcessor::SetValuesFromSettings()
{
    QStringList keys = settings->allKeys();
    verbatim = settings->value("verbatim", QVariant(false)).toBool();
    writePGCEditPal = settings->value("writePGCEditPal", QVariant(false)).toBool();
    mergePTSdiff = settings->value("mergePTSdiff", QVariant(18000)).toInt();
    alphaCrop = settings->value("alphaCrop", QVariant(14)).toInt();
    fixZeroAlpha = settings->value("fixZeroAlpha", QVariant(false)).toBool();
    scalingFilter = (ScalingFilters)scalingFilters.indexOf(settings->value("filter", QVariant(scalingFilters[(int)ScalingFilters::BILINEAR])).toString());
    scaleFilter = Filters::getFilter(scalingFilter);
    paletteMode = (PaletteMode)paletteModeNames.indexOf(settings->value("paletteMode", QVariant(paletteModeNames[(int)PaletteMode::KEEP_EXISTING])).toString());
    outMode = (OutputMode)modes.indexOf(settings->value("outputMode", QVariant(modes[(int)OutputMode::VOBSUB])).toString());
    for (QString key : settings->allKeys())
    {
        if (key.contains("recent"))
        {
            QString value = settings->value(key, QVariant("")).toString();
            if (value != "")
            {
                recentFiles.push_back(value);
            }
        }
    }
    convertResolution = settings->value("convertResolution", QVariant(convertResolution)).toBool();

    if (convertResolution)
    {
        resolutionTrg = (Resolution)resolutionNames.indexOf(settings->value("resolutionTrg", QVariant(resolutionNames[(int)resolutionTrg])).toString());
    }

    convertFPS = settings->value("convertFPS", QVariant(convertFPS)).toBool();

    if (convertFPS)
    {
        if (!fpsSrcCertain)
        {
            fpsSrc = settings->value("fpsSrc", QVariant(fpsSrc)).toBool();
        }
        fpsTrg = settings->value("fpsTrg", QVariant(fpsTrg)).toBool();
    }

    delayPTS = settings->value("delayPTS", QVariant(delayPTS)).toInt();
    fixShortFrames = settings->value("fixShortFrames", QVariant(fixShortFrames)).toBool();
    minTimePTS = settings->value("minTimePTS", QVariant(minTimePTS)).toInt();
    applyFreeScale = settings->value("applyFreeScale", QVariant(applyFreeScale)).toBool();
    if (applyFreeScale)
    {
        freeScaleX = settings->value("freeScaleX", QVariant(freeScaleX)).toDouble();
        freeScaleY = settings->value("freeScaleY", QVariant(freeScaleY)).toDouble();
    }
}

QByteArray SubtitleProcessor::getFileID(QString fileName, int numberOfBytes)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (file->size() < numberOfBytes || !file->open(QIODevice::ReadOnly))
    {
        return QByteArray();
    }
    QByteArray buf = file->read(4);
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
    if (substream.isNull())
    {
        return 0;
    }
    return substream->getNumFrames();
}

int SubtitleProcessor::getNumForcedFrames()
{
    if (substream.isNull())
    {
        return 0;
    }
    return substream->getNumForcedFrames();
}

QVector<int> &SubtitleProcessor::getFrameAlpha(int index)
{
    if (inMode == InputMode::VOBSUB)
    {
        return subDVD->getFrameAlpha(index);
    }
    return supDVD->getFrameAlpha(index);
}

QVector<int> SubtitleProcessor::getOriginalFrameAlpha(int index)
{
    if (inMode == InputMode::VOBSUB)
    {
        return subDVD->getOriginalFrameAlpha(index);
    }
    return supDVD->getOriginalFrameAlpha(index);
}

QVector<int> &SubtitleProcessor::getFramePal(int index)
{
    if (inMode == InputMode::VOBSUB)
    {
        return subDVD->getFramePal(index);
    }
    return supDVD->getFramePal(index);
}

QVector<int> SubtitleProcessor::getOriginalFramePal(int index)
{
    if (inMode == InputMode::VOBSUB)
    {
        return subDVD->getOriginalFramePal(index);
    }
    return supDVD->getOriginalFramePal(index);
}

void SubtitleProcessor::storeFreeScale(double xScale, double yScale)
{
    if (xScale < minScale)
    {
        xScale = minScale;
    }
    else if (xScale >maxScale)
    {
        xScale = maxScale;
    }
    settings->setValue("freeScaleX", QVariant(xScale));
    if (yScale < minScale)
    {
        yScale = minScale;
    }
    else if (yScale > maxScale)
    {
        yScale = maxScale;
    }
    settings->setValue("freeScaleY", QVariant(yScale));
}

void SubtitleProcessor::storeSettings()
{
    if (settings != 0)
    {
        settings->sync();
    }
}

QImage SubtitleProcessor::getSrcImage()
{
    return substream->getImage();
}

QImage SubtitleProcessor::getSrcImage(int index)
{
    substream->decode(index);
    return substream->getImage();
}

QImage SubtitleProcessor::getTrgImagePatched(SubPicture *subPicture)
{
    if (!subPicture->erasePatch.isEmpty())
    {
        Bitmap trgBitmapPatched(trgBitmapUnpatched);
        int color = trgPal.transparentIndex();
        for (auto erasePatch : subPicture->erasePatch)
        {
            trgBitmapPatched.fillRect(erasePatch->x(), erasePatch->y(), erasePatch->width(),
                                      erasePatch->height(), color);
        }
        return trgBitmapPatched.image(trgPal);
    }
    else
    {
        return trgBitmapUnpatched.image(trgPal);
    }
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

SubPicture *SubtitleProcessor::getSubPictureTrg(int index)
{
    return subPictures.at(index);
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
    if (!substream.isNull())
    {
        substream.clear();
    }
    if (!subDVD.isNull())
    {
        subDVD.clear();
    }
    if (!supDVD.isNull())
    {
        supDVD.clear();
    }
    if (!supXML.isNull())
    {
        supXML.clear();
    }
    if (!supHD.isNull())
    {
        supHD.clear();
    }
    if (!supBD.isNull())
    {
        supBD.clear();
    }
}

void SubtitleProcessor::exit()
{
    storeSettings();
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
        resolutionTrg = getResolution(getSubPictureSrc(0)->width(), getSubPictureSrc(0)->height());
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
        long ts = picSrc->startTime();
        long te = picSrc->endTime();
        // copy time stamps and apply speedup/speeddown
        if (!convertFPS)
        {
            subPictures.at(i)->setStartTime(ts + delayPTS);
            subPictures.at(i)->setEndTime(te + delayPTS);
        }
        else
        {
            subPictures.at(i)->setStartTime((long)((ts * factTS) + 0.5) + delayPTS);
            subPictures.at(i)->setEndTime((long)((te * factTS) + 0.5) + delayPTS);
        }
        // synchronize to target frame rate
        subPictures.at(i)->setStartTime(syncTimePTS(subPictures.at(i)->startTime(), fpsTrg));
        subPictures.at(i)->setEndTime(syncTimePTS(subPictures.at(i)->endTime(), fpsTrg));

        // set forced flag
        SubPicture* picTrg = subPictures.at(i);
        switch ((int)forceAll)
        {
            case (int)SetState::SET:
                picTrg->setForced(true);
                break;
            case (int)SetState::CLEAR:
                picTrg->setForced(false);
                break;
        }

        double scaleX;
        double scaleY;
        if (convertResolution)
        {
            // adjust image sizes and offsets
            // determine scaling factors
            picTrg->setWidth(getResolution(resolutionTrg)[0]);
            picTrg->setHeight(getResolution(resolutionTrg)[1]);
            scaleX = (double)picTrg->width() / picSrc->width();
            scaleY = (double)picTrg->height() / picSrc->height();
        }
        else
        {
            picTrg->setWidth(picSrc->width());
            picTrg->setHeight(picSrc->height());
            scaleX = 1.0;
            scaleY = 1.0;
        }

        int width = (int)(((picSrc->getImageWidth() * scaleX) * fx) + 0.5);
        if (width < minDim)
        {
            width = picSrc->getImageWidth();
        }
        else if (width > picTrg->width())
        {
            width = picTrg->width();
        }

        int height = (int)(((picSrc->getImageHeight() * scaleY) * fy) + 0.5);
        if (height < minDim)
        {
            height = picSrc->getImageHeight();
        }
        else if (height > picTrg->height())
        {
            height = picTrg->height();
        }

        picTrg->setImageWidth(width);
        picTrg->setImageHeight(height);

        int xOfs = (int)((picSrc->getOfsX() * scaleX) + 0.5);
        int spaceSrc = (int)(((picSrc->width() - picSrc->getImageWidth()) * scaleX) + 0.5);
        int spaceTrg = picTrg->width() - width;
        xOfs += ((spaceTrg - spaceSrc) / 2);
        if (xOfs < 0)
        {
            xOfs = 0;
        }
        else if (xOfs + width > picTrg->width())
        {
            xOfs = picTrg->width() - width;
        }
        picTrg->setOfsX(xOfs);

        int yOfs = (int)(picSrc->getOfsY() * scaleY + 0.5);
        spaceSrc = (int)((picSrc->height() - picSrc->getImageHeight()) * scaleY + 0.5);
        spaceTrg = picTrg->height() - height;
        yOfs += ((spaceTrg - spaceSrc) / 2);
        if (yOfs+height > picTrg->height())
        {
            yOfs = picTrg->height() - height;
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

void SubtitleProcessor::reScanSubtitles(Resolution oldResolution, double fpsTrgOld, int delayOld, bool convertFpsOld, double fsXOld, double fsYOld)
{
    SubPicture* picOld;
    SubPicture* picSrc;
    double factTS = 1.0;
    double factX;
    double factY;
    double fsXNew = 1.0, fsYNew = 1.0;

    if (applyFreeScale)
    {
        fsXNew = freeScaleX;
        fsYNew = freeScaleY;
    }

    if (convertFPS && !convertFpsOld)
    {
        factTS = fpsSrc / fpsTrg;
    }
    else if (!convertFPS && convertFpsOld)
    {
        factTS = fpsTrgOld / fpsSrc;
    }
    else if (convertFPS && convertFpsOld && (fpsTrg != fpsTrgOld))
    {
        factTS = fpsTrgOld / fpsTrg;
    }
    else
    {
        factTS = 1.0;
    }

    // change target resolution to source resolution if no conversion is needed
    if (!convertResolution && getNumberOfFrames() > 0)
    {
        resolutionTrg = getResolution(getSubPictureSrc(0)->width(), getSubPictureSrc(0)->height());
    }

    if (oldResolution != resolutionTrg)
    {
        QVector<int> rOld = getResolution(oldResolution);
        QVector<int> rNew = getResolution(resolutionTrg);
        factX = (double)rNew[0] / (double)rOld[0];
        factY = (double)rNew[1] / (double)rOld[1];
    }
    else
    {
        factX = 1.0;
        factY = 1.0;
    }

    // first run: clone source subpics, apply speedup/down,
    for (int i = 0; i < subPictures.size(); ++i)
    {
        picOld = subPictures[i];
        picSrc = substream->getSubPicture(i);
        subPictures[i] = picOld->copy();

        // set forced flag
        switch ((int)forceAll)
        {
        case (int)SetState::SET:
        {
            subPictures[i]->setForced(true);
        } break;
        case (int)SetState::CLEAR:
        {
            subPictures[i]->setForced(false);
        } break;
        }

        long ts = picOld->startTime();
        long te = picOld->endTime();
        // copy time stamps and apply speedup/speeddown
        if (factTS == 1.0)
        {
            subPictures[i]->setStartTime((ts - delayOld) + delayPTS);
            subPictures[i]->setEndTime((te - delayOld) + delayPTS);
        }
        else
        {
            subPictures[i]->setStartTime((long)(((ts * factTS) + 0.5) - delayOld) + delayPTS);
            subPictures[i]->setEndTime((long)(((te * factTS) + 0.5) - delayOld) + delayPTS);
        }
        // synchronize to target frame rate
        subPictures[i]->setStartTime(syncTimePTS(subPictures[i]->startTime(), fpsTrg));
        subPictures[i]->setEndTime(syncTimePTS(subPictures[i]->endTime(), fpsTrg));
        // adjust image sizes and offsets
        // determine scaling factors
        double scaleX;
        double scaleY;
        if (convertResolution)
        {
            subPictures[i]->setWidth(getResolution(resolutionTrg)[0]);
            subPictures[i]->setHeight(getResolution(resolutionTrg)[1]);
            scaleX = (double)subPictures[i]->width() / picSrc->width();
            scaleY = (double)subPictures[i]->height() / picSrc->height();
        }
        else
        {
            subPictures[i]->setWidth(picSrc->width());
            subPictures[i]->setHeight(picSrc->height());
            scaleX = 1.0;
            scaleY = 1.0;
        }

        int w = (int)(((picSrc->getImageWidth() * scaleX) * fsXNew) + 0.5);
        if (w < minDim)
        {
            w = picSrc->getImageWidth();
        }
        else if (w > subPictures[i]->width())
        {
            w = subPictures[i]->width();
            fsXNew = ((double)w / (double)picSrc->getImageWidth()) / scaleX;
        }
        int h = (int)(((picSrc->getImageHeight() * scaleY) * fsYNew) + 0.5);
        if (h < minDim)
        {
            h = picSrc->getImageHeight();
        }
        else if (h > subPictures[i]->height())
        {
            h = subPictures[i]->height();
            fsYNew = ((double)h / (double)picSrc->getImageHeight()) / scaleY;
        }

        subPictures[i]->setImageWidth(w);
        subPictures[i]->setImageHeight(h);

        // correct ratio change
        int xOfs = (int)((picOld->getOfsX() * factX) + 0.5);
        if (fsXNew != fsXOld)
        {
            int spaceTrgOld = (int)(((picOld->width() - picOld->getImageWidth()) * factX) + 0.5);
            int spaceTrg = subPictures[i]->width() - w;
            xOfs += (spaceTrg - spaceTrgOld) / 2;
        }
        if (xOfs < 0)
        {
            xOfs = 0;
        }
        else if ((xOfs + w) > subPictures[i]->width())
        {
            xOfs = subPictures[i]->width() - w;
        }
        subPictures[i]->setOfsX(xOfs);

        int yOfs = (int)((picOld->getOfsY() * factY) + 0.5);
        if (fsYNew != fsYOld)
        {
            int spaceTrgOld = (int)(((picOld->height() - picOld->getImageHeight()) * factY) + 0.5);
            int spaceTrg = subPictures[i]->height() - h;
            yOfs += (spaceTrg - spaceTrgOld) / 2;
        }
        if (yOfs < 0)
        {
            yOfs = 0;
        }
        if ((yOfs + h) > subPictures[i]->height())
        {
            yOfs = subPictures[i]->height() - h;
        }
        subPictures[i]->setOfsY(yOfs);

        // fix erase patches
        double fx = (factX * fsXNew) / fsXOld;
        double fy = (factY * fsYNew) / fsYOld;
        for (auto ep : subPictures[i]->erasePatch)
        {
            ep->setX((int)((ep->x() * fx) + 0.5));
            ep->setY((int)((ep->y() * fy) + 0.5));
            ep->setWidth((int)((ep->width() * fx) + 0.5));
            ep->setHeight((int)((ep->height() * fy) + 0.5));
        }
    }

    // 2nd run: validate times (not fully necessary, but to avoid overlap due to truncation
    SubPicture* subPicPrev = 0;
    SubPicture* subPicNext;

    for (int i = 0; i < subPictures.size(); ++i)
    {
        if (i < subPictures.size() - 1)
        {
            subPicNext = subPictures[i + 1];
        }
        else
        {
            subPicNext = 0;
        }

        picOld = subPictures[i];
        validateTimes(i, subPictures[i], subPicNext, subPicPrev);
        subPicPrev = picOld;
    }
}

QVector<int> SubtitleProcessor::getResolution(Resolution resolution)
{
    return resolutions.at((int) resolution);
}

void SubtitleProcessor::validateTimes(int index, SubPicture *subPicture, SubPicture *subPictureNext, SubPicture *subPicturePrevious)
{
    long startTime = subPicture->startTime();    // start time
    long endTime = subPicture->endTime();        // end time
    long delay = 5000 * 90;                    // default delay for missing end time (5 seconds)

    index += 1; // only used for display

    // get end time of last frame
    long lastFrameEndTime;
    if (subPicturePrevious != 0)
    {
        lastFrameEndTime = subPicturePrevious->endTime();
    }
    else
    {
        lastFrameEndTime = -1;
    }

    if (startTime < lastFrameEndTime)
    {
        printWarning(QString("start time of frame %1 < end of last frame -> fixed\n").arg(QString::number(index)));

        startTime = lastFrameEndTime;
    }

    // get start time of next frame
    long nextFrameStartTime;
    if (subPictureNext != 0)
    {
        nextFrameStartTime = subPictureNext->startTime();
    }
    else
    {
        nextFrameStartTime = 0;
    }

    if (nextFrameStartTime == 0)
    {
        if (endTime > startTime)
        {
            nextFrameStartTime = endTime;
        }
        else
        {
            // completely fucked:
            // end time and next start time are invalid
            nextFrameStartTime = startTime + delay;
        }
    }

    if (endTime <= startTime)
    {
        if (endTime == 0)
        {
            printWarning(QString("missing end time of frame %1 -> fixed\n").arg(QString::number(index)));
        }
        else
        {
            printWarning(QString("end time of frame %1 <= start time -> fixed\n").arg(QString::number(index)));
        }
        endTime = startTime + delay;
        if (endTime > nextFrameStartTime)
        {
            endTime = nextFrameStartTime;
        }
    }
    else if (endTime > nextFrameStartTime)
    {
        printWarning(QString("end time of frame %1 > start time of next frame -> fixed\n").arg(QString::number(index)));
        endTime = nextFrameStartTime;
    }

    if ((endTime - startTime) < minTimePTS)
    {
        if (fixShortFrames)
        {
            endTime = startTime + minTimePTS;
            if (endTime > nextFrameStartTime)
            {
                endTime = nextFrameStartTime;
            }
            printWarning(QString("duration of frame %1 was shorter than %2ms -> fixed\n")
                         .arg(QString::number(index)).arg(QString::number(minTimePTS / 90.0, 'g', 6)));
        }
        else
        {
            printWarning(QString("duration of frame %1 is shorter than %2ms\n")
                         .arg(QString::number(index)).arg(QString::number(minTimePTS / 90.0, 'g', 6)));
        }
    }

    if (subPicture->startTime() != startTime)
    {
        subPicture->setStartTime(syncTimePTS(startTime, fpsTrg));
    }
    if (subPicture->endTime() != endTime)
    {
        subPicture->setEndTime(syncTimePTS(endTime, fpsTrg));
    }
}

void SubtitleProcessor::readSubtitleStream()
{
    QString returnMessage;

    emit windowTitleChanged(progNameVer + " - " + QDir::toNativeSeparators(fileName));

    QFileInfo fileInfo(fileName);
    QString extension = fileInfo.suffix().toLower();
    bool isXML(extension == "xml"), isIDX(extension == "idx"), isIFO(extension == "ifo");

    QByteArray id = getFileID(fileName, 4);
    StreamID streamID = (id.isEmpty()) ? StreamID::UNKNOWN : getStreamID(id);

    maxProgress = fileInfo.size();
    lastProgress = 0;

    emit progressDialogTitleChanged(QString("Loading"));
    emit progressDialogTextChanged(QString("Loading subtitle stream"));
    emit progressDialogVisibilityChanged(true);

    try
    {
        if (isXML || streamID == StreamID::XML)
        {
            readXml();
        }
        else if (isIDX || streamID == StreamID::DVDSUB || streamID == StreamID::IDX)
        {
            readDVDSubStream(streamID, true);
        }
        else if (isIFO || streamID == StreamID::IFO || !ifoFile.isEmpty())
        {
            readDVDSubStream(streamID, false);
        }
        else if (streamID == StreamID::BDSUP)
        {
            readSup();
        }
        else
        {
            if (QFileInfo(QString("%1/%2.ifo").arg(fileInfo.absolutePath()).arg(fileInfo.completeBaseName())).exists())
            {
                readDVDSubStream(StreamID::IFO, false);
            }
            else
            {
                try
                {
                    readDVDSubStream(streamID, false);
                }
                catch(QString e)
                {
                    printX("SUP file was not a valid DVD SUP file. Attempting to load as HD DVD SUP.\n");
                    readSup();
                }
            }
        }
    }
    catch (QString e)
    {
        returnMessage = e;
    }

    emit progressDialogVisibilityChanged(false);
    emit loadingSubtitleFinished(returnMessage);
}

void SubtitleProcessor::createSubtitleStream()
{
    QString returnMessage;
    maxProgress = substream->getNumFrames();
    lastProgress = 0;
    setCurrentProgress(0);

    emit progressDialogTitleChanged("Exporting");
    if (outMode == OutputMode::VOBSUB)
    {
        emit progressDialogTextChanged("Exporting SUB/IDX");
    }
    else if (outMode == OutputMode::BDSUP)
    {
        emit progressDialogTextChanged("Exporting SUP(BD)");
    }
    else if (outMode == OutputMode::XML)
    {
        emit progressDialogTextChanged("Exporting XML/PNG");
    }
    else
    {
        emit progressDialogTextChanged("Exporting SUP/IFO");
    }
    emit progressDialogVisibilityChanged(true);

    try
    {
        writeSub(fileName);
    }
    catch (QString e)
    {
        returnMessage = e;
    }

    emit progressDialogVisibilityChanged(false);
    emit writingSubtitleFinished(returnMessage);
}

void SubtitleProcessor::writeSub(QString filename)
{
    QScopedPointer<QFile> out;
    QVector<int> offsets;
    QVector<int> timestamps;
    int frameNum = 0;
    int maxNum;
    QString fn = "";

    // handling of forced subtitles
    if (exportForced)
    {
        maxNum = countForcedIncluded();
    }
    else
    {
        maxNum = countIncluded();
    }

    QFileInfo fileInfo(filename);

    // handle file name extensions depending on mode
    if (outMode == OutputMode::VOBSUB)
    {
        filename = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".sub";
        out.reset(new QFile(filename));
        if (!out->open(QIODevice::WriteOnly))
        {
            throw QString("File: '%1' can not be opened for reading.").arg(filename);
        }
    }
    else if (outMode == OutputMode::SUPIFO || outMode == OutputMode::BDSUP)
    {
        filename = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".sup";
        out.reset(new QFile(filename));
        if (!out->open(QIODevice::WriteOnly))
        {
            throw QString("File: '%1' can not be opened for reading.").arg(filename);
        }
    }
    else
    {
        fn = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName();
        filename = fn + ".xml";
    }

    printX(QString("\nWriting %1\n").arg(filename));

    numberOfErrors = 0;
    numberOfWarnings = 0;

    // main loop
    int offset = 0;
    offsets.push_back(offset);
    for (int i = 0; i < substream->getNumFrames(); ++i)
    {
        if (isCancelled())
        {
            throw QString("Cancelled by user!");
        }
        setCurrentProgress(i);

        if (!subPictures[i]->exclude() && (!exportForced || subPictures[i]->isForced()))
        {
            if (outMode == OutputMode::VOBSUB)
            {
                convertSup(i, (frameNum / 2) + 1, maxNum);
                subVobTrg->copyInfo(subPictures[i]);
                if (subDVD.isNull())
                {
                    subDVD = QSharedPointer<SubDVD>(new SubDVD("", "", this));
                }
                QVector<uchar> buf = subDVD->createSubFrame(*subVobTrg, trgBitmap);
                out->write(QByteArray((char*)buf.data(), buf.size()));
                offset += buf.size();
                offsets.push_back(offset);
                timestamps.push_back((int)subPictures[i]->startTime());
            }
            else if (outMode == OutputMode::SUPIFO)
            {
                convertSup(i, (frameNum / 2) + 1, maxNum);
                subVobTrg->copyInfo(subPictures[i]);
                if (supDVD.isNull())
                {
                    supDVD = QSharedPointer<SupDVD>(new SupDVD("", "", this));
                }
                QVector<uchar> buf = supDVD->createSupFrame(*subVobTrg, trgBitmap);
                out->write(QByteArray((char*)buf.data(), buf.size()));
            }
            else if (outMode == OutputMode::BDSUP)
            {
                subPictures[i]->setCompNum(frameNum);
                convertSup(i, (frameNum / 2) + 1, maxNum);
                if (supBD.isNull())
                {
                    supBD = QSharedPointer<SupBD>(new SupBD("", this));
                }
                QVector<uchar> buf = supBD->createSupFrame(subPictures[i], trgBitmap, trgPal);
                out->write(QByteArray((char*)buf.data(), buf.size()));
            }
            else
            {
                // Xml
                convertSup(i, (frameNum / 2) + 1, maxNum);
                if (supXML.isNull())
                {
                    supXML = QSharedPointer<SupXML>(new SupXML("", this));
                }
                QString fnp = supXML->getPNGname(fn, i + 1);
                trgBitmap.image(trgPal).save(fnp, "PNG");
            }
            frameNum += 2;
        }
    }

    bool importedDVDPalette;
    if (inMode == InputMode::VOBSUB || inMode == InputMode::SUPIFO)
    {
        importedDVDPalette = true;
    }
    else
    {
        importedDVDPalette = false;
    }

    Palette trgPallete(0, true);
    if (outMode == OutputMode::VOBSUB)
    {
        // VobSub - write IDX
        /* return offets as array of ints */
        QVector<int> ofs(offsets.size());
        for (int i = 0; i < ofs.size(); ++i)
        {
            ofs.replace(i, offsets[i]);
        }
        QVector<int> ts(timestamps.size());
        for (int i = 0; i < ts.size(); ++i)
        {
            ts.replace(i, timestamps[i]);
        }
        filename = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".idx";

        printX(QString("\nWriting %1\n").arg(filename));

        if (!importedDVDPalette || paletteMode != PaletteMode::KEEP_EXISTING)
        {
            trgPallete = currentDVDPalette;
        }
        else
        {
            trgPallete = currentSourceDVDPalette;
        }
        subDVD->writeIdx(filename, *subPictures[0], ofs, ts, trgPallete);
    }
    else if (outMode == OutputMode::XML)
    {
        printX(QString("\nWriting %1\n").arg(filename));

        supXML->writeXml(filename, subPictures);
    }
    else if (outMode == OutputMode::SUPIFO)
    {
        // SUP/IFO - write IFO
        if (!importedDVDPalette || paletteMode != PaletteMode::KEEP_EXISTING)
        {
            trgPallete = currentDVDPalette;
        }
        else
        {
            trgPallete = currentSourceDVDPalette;
        }
        filename = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".ifo";

        printX(QString("\nWriting %1\n").arg(filename));

        supDVD->writeIfo(filename, *subPictures[0], trgPallete);
    }

    // only possible for SUB/IDX and SUP/IFO (else there is no public palette)
    if (trgPallete.size() != 0 && writePGCEditPal)
    {
        QString fnp = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".txt";

        printX(QString("\nWriting %1\n").arg(fnp));

        writePGCEditPalette(fnp, trgPallete);
    }
}

void SubtitleProcessor::moveAll()
{
    QString returnMessage;

    maxProgress = substream->getNumFrames();
    lastProgress = 0;

    emit progressDialogTitleChanged("Moving");
    emit progressDialogTextChanged("Moving all captions");
    emit progressDialogVisibilityChanged(true);

    try
    {
        moveAllToBounds();
    }
    catch (QString e)
    {
        returnMessage = e;
    }

    emit progressDialogVisibilityChanged(false);
    emit moveAllFinished(returnMessage);
}

double SubtitleProcessor::getDefaultFPS(Resolution resolution)
{
    switch ((int)resolution)
    {
        case 0:
            return FPS_NTSC;
        case 1:
            return FPS_PAL;
        default:
            return FPS_24P;
    }
}

void SubtitleProcessor::print(const QString &message)
{
    if (!verbatim) return;
    if (parent != 0)
    {
        emit printText(message);
    }
    else
    {
        fprintf(stdout, message.toAscii());
    }
}

void SubtitleProcessor::printX(const QString &message)
{
    if (parent != 0)
    {
        emit printText(message);
    }
    else
    {
        fprintf(stdout, message.toAscii());
    }
}

void SubtitleProcessor::printError(const QString &message)
{
    ++numberOfErrors;
    if (parent != 0)
    {
        emit printText(QString("ERROR: " + message));
    }
    else
    {
        fprintf(stdout, (QString("ERROR: " + message)).toAscii());
    }
}

void SubtitleProcessor::printWarning(const QString &message)
{
    ++numberOfWarnings;
    if (parent != 0)
    {
        emit printText(QString("WARNING: " + message));
    }
    else
    {
        fprintf(stdout, (QString("WARNING: " + message)).toAscii());
    }
}

void SubtitleProcessor::onLanguageRead(const QString &message)
{
    emit addLanguage(message);
}

void SubtitleProcessor::moveAllToBounds()
{
    QString sy("");
    switch ((int)moveModeY)
    {
    case (int)MoveModeY::INSIDE:
    {
        sy = "inside cinemascope bars";
    } break;
    case (int)MoveModeY::OUTSIDE:
    {
        sy = "outside cinemascope bars";
    } break;
    case (int)MoveModeY::ORIGIN:
    {
        sy = "from the original Y position";
    } break;
    }

    QString sx("");
    switch ((int)moveModeX)
    {
    case (int)MoveModeX::CENTER:
    {
        sx = "to the center vertically";
    } break;
    case (int)MoveModeX::LEFT:
    {
        sx = "to the left";
    } break;
    case (int)MoveModeX::RIGHT:
    {
        sx = "to the right";
    } break;
    case (int)MoveModeX::ORIGIN:
    {
        sx = "from the original X position";
    } break;
    }

    QString s("Moving captions ");
    if (!sy.isEmpty())
    {
        s += sy + "";
        if (!sx.isEmpty())
        {
            s += " and " + sx;
        }
        print(s+".\n");
    }
    else if (!sx.isEmpty())
    {
        print(s+sx+".\n");
    }
    if (!cliMode)
    {
        // in CLI mode, moving is done during export
        for (int idx = 0; idx < subPictures.size(); ++idx)
        {
            setCurrentProgress(idx);
            if (!subPictures.at(idx)->wasDecoded())
            {
                convertSup(idx, idx + 1, subPictures.size(), true);
            }
            moveToBounds(subPictures.at(idx), idx + 1, cineBarFactor, moveOffsetX, moveOffsetY, moveModeX, moveModeY, cropOfsY);
        }
    }
}

void SubtitleProcessor::convertSup(int index, int displayNumber, int displayMax, bool skipScaling)
{
    int width, height;
    int startOfs = (int)substream->getStartOffset(index);
    SubPicture* subPic = substream->getSubPicture(index);

    printX(QString("Decoding frame %1/%2%3")
           .arg(QString::number(displayNumber)).arg(QString::number(displayMax))
           .arg(substream == supXML ? QString("\n") : QString(" at offset %1\n").arg(QString::number(startOfs, 16), 8, QChar('0'))));

    substream->decode(index);
    width = subPic->getImageWidth();
    height = subPic->getImageHeight();
    if (outMode == OutputMode::VOBSUB || outMode == OutputMode::SUPIFO)
    {
        determineFramePalette(index);
    }
    updateTrgPic(index);
    SubPicture* picTrg = subPictures.at(index);
    picTrg->setDecoded(true);

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
        Bitmap targetBitmap;
        Palette targetPalette = trgPal;
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
                    targetBitmap = substream->getBitmap().convertLm(substream->getPalette(), alphaThreshold, luminanceThreshold); // reduce palette
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
                        targetBitmap = substream->getBitmap().scaleFilter(trgWidth, trgHeight, substream->getPalette(), *scaleFilter);
                    }
                    else
                    {
                        targetBitmap = substream->getBitmap().scaleBilinear(trgWidth, trgHeight, substream->getPalette());
                    }
                }
                else
                {
                    // reduce palette
                    if (scaleFilter != 0)
                    {
                        targetBitmap = substream->getBitmap().scaleFilterLm(trgWidth, trgHeight, substream->getPalette(), alphaThreshold, luminanceThreshold, *scaleFilter);
                    }
                    else
                    {
                        targetBitmap = substream->getBitmap().scaleBilinearLm(trgWidth, trgHeight, substream->getPalette(), alphaThreshold, luminanceThreshold);
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
                        targetBitmap = substream->getBitmap().scaleFilter(trgWidth, trgHeight, substream->getPalette(), *scaleFilter);
                    }
                    else
                    {
                        targetBitmap = substream->getBitmap().scaleBilinear(trgWidth, trgHeight, substream->getPalette());
                    }
                }
                else
                {
                    // create new palette
                    bool dither = paletteMode == PaletteMode::CREATE_DITHERED;
                    PaletteBitmap paletteBitmap;
                    if (scaleFilter != 0)
                    {
                        paletteBitmap = substream->getBitmap().scaleFilter(trgWidth, trgHeight, substream->getPalette(), *scaleFilter, dither);
                    }
                    else
                    {
                        paletteBitmap = substream->getBitmap().scaleBilinear(trgWidth, trgHeight, substream->getPalette(), dither);
                    }
                    targetBitmap = paletteBitmap.bitmap;
                    targetPalette = &paletteBitmap.palette;
                }
            }
        }
        if (!picTrg->erasePatch.isEmpty())
        {
            trgBitmapUnpatched = Bitmap(targetBitmap);
            int col = targetPalette.transparentIndex();
            for (auto erasePatch : picTrg->erasePatch)
            {
                targetBitmap.fillRect(erasePatch->x(), erasePatch->y(), erasePatch->width(),
                                      erasePatch->height(), col);
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
        QRgb rgbSrc(substream->getPalette().rgb(substream->getPrimaryColorIndex()));

        // match with primary color from 16 color target palette
        // note: skip index 0 , primary colors at even positions
        // special treatment for index 1:  white
        Palette trgPalette = currentDVDPalette;
        int minDistance = 0xffffff; // init > 0xff*0xff*3 = 0x02fa03
        int colIdx = 0;
        for (int idx = 1; idx < trgPalette.size(); idx += 2 )
        {
            QRgb rgb(trgPalette.rgb(idx));
            // distance vector (skip sqrt)
            int rd = qRed(rgbSrc) - qRed(rgb);
            int gd = qGreen(rgbSrc) - qGreen(rgb);
            int bd = qBlue(rgbSrc) - qBlue(rgb);
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

        trgPal = SubstreamDVD::decodePalette(*subVobTrg, trgPalette, alphaCrop);
    }
    else
    {
        SubstreamDVD* substreamDVD;
        // use palette from loaded VobSub or SUP/IFO
        Palette miniPal(4, true);
        QVector<int> alpha;
        QVector<int> paletteFrame;

        if (inMode == InputMode::VOBSUB)
        {
            substreamDVD = subDVD.data();
        }
        else
        {
            substreamDVD = supDVD.data();
        }

        alpha = substreamDVD->getFrameAlpha(index);
        paletteFrame = substreamDVD->getFramePal(index);

        for (int i = 0; i < 4; ++i)
        {
            int a = (alpha[i] * 0xff) / 0xf;
            if (a >= alphaCrop)
            {
                miniPal.setARGB(i, currentSourceDVDPalette.rgba(paletteFrame[i]));
                miniPal.setAlpha(i, a);
            }
            else
            {
                miniPal.setARGB(i, 0);
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
    double scaleX = (double)picTrg->width() / picSrc->width();
    double scaleY = (double)picTrg->height() / picSrc->height();
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
    else if (wNew > picTrg->width())
    {
        wNew = picTrg->width();
    }
    int hNew = (int)(((picSrc->getImageHeight() * scaleY) * fy) + 0.5);
    if (hNew < minDim)
    {
        hNew = picSrc->getImageHeight();
    }
    else if (hNew > picTrg->height())
    {
        hNew = picTrg->height();
    }
    picTrg->setImageWidth(wNew);
    picTrg->setImageHeight(hNew);
    if (wNew != wOld)
    {
        int xOfs = (int)((picSrc->getOfsX() * scaleX) + 0.5);
        int spaceSrc = (int)(((picSrc->width() - picSrc->getImageWidth()) * scaleX) + 0.5);
        int spaceTrg = picTrg->width() - wNew;
        xOfs += (spaceTrg - spaceSrc) / 2;
        if (xOfs < 0)
        {
            xOfs = 0;
        }
        else if (xOfs + wNew > picTrg->width())
        {
            xOfs = picTrg->width() - wNew;
        }
        picTrg->setOfsX(xOfs);
    }
    if (hNew != hOld)
    {
        int yOfs = (int)((picSrc->getOfsY() * scaleY) + 0.5);
        int spaceSrc = (int)(((picSrc->height() - picSrc->getImageHeight()) * scaleY) + 0.5);
        int spaceTrg = picTrg->height() - hNew;
        yOfs += (spaceTrg - spaceSrc) / 2;
        if (yOfs + hNew > picTrg->height())
        {
            yOfs = picTrg->height() - hNew;
        }
        picTrg->setOfsY(yOfs);
    }
    // was image cropped?
    return (wNew != wOld) || (hNew != hOld);
}

void SubtitleProcessor::addRecent(QString fileName)
{
    bool notFound = recentFiles.indexOf(fileName) == -1;
    if (notFound)
    {
        recentFiles.insert(recentFiles.begin(), fileName);
        if (recentFiles.size() > 5)
        {
            recentFiles.removeLast();
        }
        for (int i = 0; i < recentFiles.size(); ++i)
        {
            settings->setValue(QString("recent_%1").arg(QString::number(i)), QVariant(recentFiles.at(i)));
        }
    }
}

void SubtitleProcessor::removeRecent(QString fileName)
{
    recentFiles.removeOne(fileName);
    int idxToRemove = -1;

    for (QString key : settings->allKeys())
    {
        if (settings->value(key, QVariant("")).toString() == fileName)
        {
            idxToRemove = settings->allKeys().indexOf(key);
        }
    }
    if (idxToRemove != -1)
    {
        settings->remove(settings->allKeys()[idxToRemove]);
        for (int i = 0; i < recentFiles.size(); ++i)
        {
            settings->setValue(QString("recent_%1").arg(QString::number(i)), QVariant(recentFiles.at(i)));
        }
        settings->sync();
    }
}

void SubtitleProcessor::moveToBounds(SubPicture *picture, int index, double barFactor, int offsetX, int offsetY, MoveModeX mmx, MoveModeY mmy, int cropOffsetY)
{
    int barHeight = (int)((picture->height() * barFactor) + 0.5);
    int y1 = picture->getOfsY();
    int h = picture->height();
    int w = picture->width();
    int hi = picture->getImageHeight();
    int wi = picture->getImageWidth();
    int y2 = y1 + hi;
    CaptionType c;

    if (mmy != MoveModeY::KEEP)
    {
        // move vertically
        if (y1 < h / 2 && y2 < h / 2)
        {
            c = CaptionType::UP;
        }
        else if (y1 > h / 2 && y2 > h / 2)
        {
            c = CaptionType::DOWN;
        }
        else
        {
            c = CaptionType::FULL;
        }

        switch ((int)c)
        {
        case (int)CaptionType::FULL:
        {
            // maybe add scaling later, but for now: do nothing
            printWarning(QString("Caption %1 not moved (too large)\n")
                         .arg(QString::number(index)));
        } break;
        case (int)CaptionType::UP:
        {
            if (mmy == MoveModeY::INSIDE)
            {
                picture->setOfsY(barHeight + offsetY);
            }
            else if (mmy == MoveModeY::OUTSIDE)
            {
                picture->setOfsY(offsetY);
            }
            else
            {
                picture->setOfsY(y1 + offsetY);
            }

            print(QString("Caption %1 moved to y position %2\n")
                  .arg(QString::number(index))
                  .arg(QString::number(picture->getOfsY())));
        } break;
        case (int)CaptionType::DOWN:
        {
            if (mmy == MoveModeY::INSIDE)
            {
                picture->setOfsY((((h - barHeight) - offsetY)) - hi);
            }
            else if (mmy == MoveModeY::OUTSIDE)
            {
                picture->setOfsY(h - offsetY - hi);
            }
            else
            {
                picture->setOfsY(y1 + offsetY);
            }
            print(QString("Caption %1 moved to y position %2\n")
                  .arg(QString::number(index))
                  .arg(QString::number(picture->getOfsY())));
        } break;
        }

        if (picture->getOfsY() < cropOffsetY)
        {
            picture->getOfsY();
        }
        else
        {
            int yMax = (picture->height() - picture->getImageHeight()) - cropOffsetY;
            if (picture->getOfsY() > yMax)
            {
                picture->setOfsY(yMax);
            }
        }
    }
    // move horizontally
    switch ((int)mmx)
    {
    case (int)MoveModeX::ORIGIN:
    {
        if ((picture->getOfsX() + offsetX) < 0)
        {
            picture->setOfsX(0);
        }
        else if ((picture->getImageWidth() + picture->getOfsX() + offsetX) > picture->width())
        {
            picture->setOfsX(picture->width() - (picture->getImageWidth() + picture->getOfsX()));
        }
        else
        {
            picture->setOfsX(picture->getOfsX() + offsetX);
        }
    } break;
    case (int)MoveModeX::LEFT:
    {
        if ((w - wi) >= offsetX)
        {
            picture->setOfsX(offsetX);
        }
        else
        {
            picture->setOfsX((w-wi)/2);
        }
    } break;
    case (int)MoveModeX::RIGHT:
    {
        if ((w - wi) >= offsetX)
        {
            picture->setOfsX((w - wi) - offsetX);
        }
        else
        {
            picture->setOfsX((w - wi) / 2);
        }
    } break;
    case (int)MoveModeX::CENTER:
    {
        picture->setOfsX((w - wi) / 2);
    } break;
    }
}

QString SubtitleProcessor::getSrcInfoStr(int index)
{
    SubPicture* pic = substream->getSubPicture(index);
    QString text = "screen size: " + QString::number(pic->width()) + "x" + QString::number(pic->height()) + "    ";
    text +=	"image size: " + QString::number(pic->getImageWidth()) + "x" + QString::number(pic->getImageHeight()) + "    ";
    text += "pos: (" + QString::number(pic->getOfsX()) + "," + QString::number(pic->getOfsY()) + ") - (" + QString::number((pic->getOfsX() + pic->getImageWidth()))
          + "," + QString::number((pic->getOfsY() + pic->getImageHeight())) + ")    ";
    text += "start: " + TimeUtil::ptsToTimeStr(pic->startTime()) + "    ";
    text += "end: " + TimeUtil::ptsToTimeStr(pic->endTime()) + "    ";
    text += "forced: " + ((pic->isForced()) ? QString("yes") : QString("no"));
    return text;
}

QString SubtitleProcessor::getTrgInfoStr(int index)
{
    SubPicture* pic = subPictures[index];
    QString text = "screen size: " + QString::number(getTrgWidth(index)) + "x" + QString::number(getTrgHeight(index)) + "    ";
    text +=	"image size: " + QString::number(getTrgImgWidth(index)) + "x" + QString::number(getTrgImgHeight(index)) + "    ";
    text += "pos: (" + QString::number(pic->getOfsX()) + "," + QString::number(pic->getOfsY()) + ") - (" + QString::number((pic->getOfsX() + getTrgImgWidth(index)))
          + "," + QString::number((pic->getOfsY() + getTrgImgHeight(index))) + ")    ";
    text += "start: " + TimeUtil::ptsToTimeStr(pic->startTime()) + "    ";
    text += "end: " + TimeUtil::ptsToTimeStr(pic->endTime()) + "    ";
    text += "forced: " + ((pic->isForced()) ? QString("yes") : QString("no"));
    return text;
}

int SubtitleProcessor::getTrgWidth(int index)
{
    return subPictures.at(index)->width();
}

int SubtitleProcessor::getTrgHeight(int index)
{
    return subPictures.at(index)->height();
}

int SubtitleProcessor::getTrgOfsX(int index)
{
    return subPictures.at(index)->getOfsX();
}

int SubtitleProcessor::getTrgOfsY(int index)
{
    return subPictures.at(index)->getOfsY();
}

QImage SubtitleProcessor::getTrgImage()
{
    return trgBitmap.image(trgPal);
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
    return subPictures.at(index)->exclude();
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
    d = ok ? d : -1.0;

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
    if (!cliMode)
    {
        this->maxProgress = maxProgress;
    }
}

void SubtitleProcessor::setCurrentProgress(int currentProgress)
{
    if (!cliMode)
    {
        int value = (int)(((long)currentProgress * 100) / maxProgress);
        if (value > lastProgress)
        {
            lastProgress = value;
            emit progressDialogValueChanged(value);
        }
    }
}

void SubtitleProcessor::readXml()
{
    printX(QString("Loading %1\n").arg(fileName));
    numberOfErrors = 0;
    numberOfWarnings = 0;

    supXML = QSharedPointer<SupXML>(new SupXML(fileName, this));
    connect(supXML.data(), SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
    connect(supXML.data(), SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
    supXML->readAllImages();
    substream = qSharedPointerCast<Substream>(supXML);

    inMode = InputMode::XML;

    // decode first frame
    substream->decode(0);
    subVobTrg = new SubPictureDVD;

    // automatically set luminance thresholds for VobSub conversion
    int maxLum = substream->getPalette().Y()[substream->getPrimaryColorIndex()] & 0xff;
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
    printX(QString("Loading %1\n").arg(fileName));
    numberOfErrors = 0;
    numberOfWarnings = 0;

    SubstreamDVD* substreamDVD;
    QFileInfo fileInfo(fileName);

    if (isVobSub)
    {
        QString idxFileName;
        QString subFileName;
        if (streamID == StreamID::DVDSUB)
        {
            subFileName = fileName;
            idxFileName = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".idx";
        }
        else
        {
            idxFileName = fileName;
            subFileName = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".sub";
        }
        subDVD = QSharedPointer<SubDVD>(new SubDVD(subFileName, idxFileName, this));
        connect(subDVD.data(), SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(subDVD.data(), SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        connect(subDVD.data(), SIGNAL(addLanguage(QString)), this, SLOT(onLanguageRead(QString)));
        subDVD->readIdx(idxToRead);
        subDVD->readAllSubFrames();
        languageIdxRead = subDVD->getLanguageIdxRead();
        substream = qSharedPointerCast<Substream>(subDVD);
        inMode = InputMode::VOBSUB;
        substreamDVD = subDVD.data();
    }
    else
    {
        QString ifoFileName;
        QString supFileName;
        if (fileInfo.suffix() == "sup")
        {
            supFileName = fileName;
            if (!ifoFile.isEmpty())
            {
                ifoFileName = ifoFile;
            }
            else
            {
                ifoFileName = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".ifo";
                if (!QFileInfo(ifoFileName).exists())
                {
                    printWarning("No IFO file specified.  Assuming 720x576 and 25fps.\n");
                    ifoFileName = supFileName;
                }
            }
        }
        else
        {
            ifoFileName = fileName;
            supFileName = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".sup";
        }
        supDVD = QSharedPointer<SupDVD>(new SupDVD(supFileName, ifoFileName, this));
        connect(supDVD.data(), SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(supDVD.data(), SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        if (ifoFileName != supFileName)
        {
            supDVD->readIfo();
        }
        supDVD->readAllSupFrames();
        substream = qSharedPointerCast<Substream>(supDVD);
        inMode = InputMode::SUPIFO;
        substreamDVD = supDVD.data();
    }

    substream->decode(0);
    subVobTrg = new SubPictureDVD;
    defaultSourceDVDPalette = Palette(substreamDVD->getSrcPalette());
    currentSourceDVDPalette = Palette(defaultSourceDVDPalette);

    int primaryColorIndex = substream->getPrimaryColorIndex();
    int yMax = substream->getPalette().Y()[primaryColorIndex] & 0xff;

    if (yMax > 10)
    {
        int yMin = yMax;
        Palette palette = substream->getPalette();
        for (int i = 0; i < 4; ++i)
        {
            int y = palette.Y()[i] & 0xff;
            int a = palette.alpha(i);
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
        int height = substream->getSubPicture(0)->height();

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
    printX(QString("Loading %1\n").arg(fileName));
    numberOfErrors = 0;
    numberOfWarnings = 0;

    for (auto languageTriple : languages)
    {
        if (fileName.contains(languageTriple[0], Qt::CaseInsensitive))
        {
            languageIdx = languages.indexOf(languageTriple);

            printX(QString("Selected language '%1 (%2)' by filename\n")
                   .arg(languageTriple[0])
                   .arg(languageTriple[1]));
            break;
        }
    }

    QByteArray id = getFileID(fileName, 2);

    if (!id.isEmpty() && ((uchar)id[0] == 0x50 && (uchar)id[1] == 0x47))
    {
        supBD = QSharedPointer<SupBD>(new SupBD(fileName, this));
        connect(supBD.data(), SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(supBD.data(), SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        supBD->readAllSupFrames();
        substream = qSharedPointerCast<Substream>(supBD);
        inMode = InputMode::BDSUP;
    }
    else
    {
        supHD = QSharedPointer<SupHD>(new SupHD(fileName, this));
        connect(supHD.data(), SIGNAL(maxProgressChanged(int)), this, SLOT(setMaxProgress(int)));
        connect(supHD.data(), SIGNAL(currentProgressChanged(int)), this, SLOT(setCurrentProgress(int)));
        supHD->readAllSupFrames();
        substream = qSharedPointerCast<Substream>(supHD);
        inMode = InputMode::HDDVDSUP;
    }

    // decode first frame
    substream->decode(0);
    subVobTrg = new SubPictureDVD;

    // automatically set luminance thresholds for VobSub conversion
    int maxLum = substream->getPalette().Y()[substream->getPrimaryColorIndex()] & 0xff;
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
        if (substream == supBD.data())
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

int SubtitleProcessor::countForcedIncluded()
{
    int n = 0;
    for (auto subPicture : subPictures)
    {
        if (subPicture->isForced() && !subPicture->exclude())
        {
            ++n;
        }
    }
    return n;
}

int SubtitleProcessor::countIncluded()
{
    int n = 0;
    for (auto subPicture : subPictures)
    {
        if (!subPicture->exclude())
        {
            ++n;
        }
    }
    return n;
}

void SubtitleProcessor::writePGCEditPalette(QString filename, Palette &palette)
{
    QScopedPointer<QFile> out(new QFile(filename));
    if (!out->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw QString("PGCEdit Palette file can not be opened for writing.");
    }
    out->write("# Palette file for PGCEdit - colors given as R,G,B components (0..255)\n");
    for (int i = 0; i < palette.size(); ++i)
    {
        QRgb rgb = palette.rgb(i);
        out->write(QString("Color " + QString::number(i) + "=" + QString::number(qRed(rgb)) +
                           ", " + QString::number(qGreen(rgb)) + ", " + QString::number(qBlue(rgb)) + "\n").toAscii());
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
    else if (resolution == Resolution::PAL)
    {
        return resolutions[1];
    }
    else if (resolution == Resolution::HD_720)
    {
        return resolutions[2];
    }
    else if (resolution == Resolution::HD_1440x1080)
    {
        return resolutions[3];
    }
    else
    {
        return resolutions[4];
    }
}

void SubtitleProcessor::setMoveModeX(MoveModeX value)
{
    moveModeX = value;
    moveCaptions = (moveModeY != MoveModeY::KEEP || moveModeX != MoveModeX::KEEP);
}

void SubtitleProcessor::setMoveModeY(MoveModeY value)
{
    moveModeY = value;
    moveCaptions = (moveModeY != MoveModeY::KEEP || moveModeX != MoveModeX::KEEP);
}

void SubtitleProcessor::setOutputMode(OutputMode mode)
{
    outMode = mode;
    if (settings != 0)
    {
        settings->setValue("outputMode", QVariant(modes[(int)mode]));
    }
}

void SubtitleProcessor::setPaletteMode(PaletteMode value)
{
    paletteMode = value;
    if (settings != 0)
    {
        settings->setValue("paletteMode", QVariant(paletteModeNames[(int)value]));
    }
}

void SubtitleProcessor::setScalingFilter(ScalingFilters value)
{
    scalingFilter = value;
    scaleFilter = Filters::getFilter(scalingFilter);
    if (settings != 0)
    {
        settings->setValue("filter", QVariant(scalingFilters[(int)value]));
    }
}
