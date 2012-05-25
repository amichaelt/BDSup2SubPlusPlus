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

#include "subdvd.h"
#include "bitmap.h"
#include "palette.h"
#include "types.h"
#include "subtitleprocessor.h"
#include "subpicturedvd.h"
#include "Tools/filebuffer.h"
#include "Tools/timeutil.h"

#include <QImage>
#include <QFile>
#include <QRect>
#include <QTextStream>
#include <QFileInfo>
#include <QVector>
#include <QMessageBox>

SubDVD::SubDVD(QString subFileName, QString idxFileName, SubtitleProcessor* subtitleProcessor) :
    idxFile(idxFileName)
{
    this->subtitleProcessor = subtitleProcessor;
    fileBuffer = new FileBuffer(subFileName);
    palette = new Palette(4, true);
    bitmap = new Bitmap(0, 0);
}

Palette *SubDVD::getPalette()
{
    return palette;
}

Bitmap *SubDVD::getBitmap()
{
    return bitmap;
}

QImage *SubDVD::getImage()
{
    return bitmap->getImage(palette);
}

QImage *SubDVD::getImage(Bitmap *bitmap)
{
    //TODO: Finish implementing
    throw 10;
}

int SubDVD::getPrimaryColorIndex()
{
    return primaryColorIndex;
}

void SubDVD::decode(int index)
{
    if (index < subPictures.size())
    {
        SubstreamDVD::decode(subPictures.at(index), subtitleProcessor);
    }
    else
    {
        //implement error handling
        return;
    }
}

int SubDVD::getNumFrames()
{
    return subPictures.size();
}

int SubDVD::getNumForcedFrames()
{
    //TODO: Finish implementing
    throw 10;
}

bool SubDVD::isForced(int index)
{
    //TODO: Finish implementing
    throw 10;
}

void SubDVD::close()
{
    fileBuffer->close();
}

long SubDVD::getEndTime(int index)
{
    //TODO: Finish implementing
    throw 10;
}

long SubDVD::getStartTime(int index)
{
    //TODO: Finish implementing
    throw 10;
}

long SubDVD::getStartOffset(int index)
{
    return subPictures.at(index)->offset;
}

SubPicture *SubDVD::getSubPicture(int index)
{
    return subPictures.at(index);
}

QVector<int> SubDVD::getFrameAlpha(int index)
{
    return subPictures.at(index)->alpha;
}

QVector<int> SubDVD::getFramePal(int index)
{
    return subPictures.at(index)->pal;
}

QVector<int> SubDVD::getOriginalFrameAlpha(int index)
{
    //TODO: Finish implementing
    throw 10;
}

QVector<int> SubDVD::getOriginalFramePal(int index)
{
    //TODO: Finish implementing
    throw 10;
}

Palette *SubDVD::getSrcPalette()
{
    return srcPalette;
}

void SubDVD::setSrcPalette(Palette *palette)
{
    //TODO: Finish implementing
    throw 10;
}

int SubDVD::getLanguageIdx()
{
    return languageIdx;
}

void SubDVD::readSubFrame(SubPictureDVD *pic, long endOfs)
{
    long ofs = pic->offset;
    long ctrlOfs = -1;
    long nextOfs;
    int  ctrlOfsRel = 0;
    int rleSize = 0;
    int rleBufferFound = 0;
    int ctrlSize = -1;
    int ctrlHeaderCopied = 0;
    QVector<uchar> ctrlHeader;
    ImageObjectFragment* rleFrag;
    int length;
    int packHeaderSize;
    bool firstPackFound = false;

    do {
        // 4 bytes:  packet identifier 0x000001ba
        long startOfs = ofs;
        if (fileBuffer->getDWord(ofs) != 0x000001ba)
        {
            //TODO: print error message
            throw 10;
        }
        // 6 bytes:  system clock reference
        // 3 bytes:  multiplexer rate
        // 1 byte:   stuffing info
        int stuffOfs = fileBuffer->getByte(ofs += 13) & 7;
        // 4 bytes:  sub packet ID 0x000001bd
        if (fileBuffer->getDWord(ofs += (1 + stuffOfs)) != 0x000001bd)
        {
            //TODO: print error message
            throw 10;
        }
        // 2 bytes:  packet length (number of bytes after this entry)
        length = fileBuffer->getWord(ofs += 4);
        nextOfs = ofs + 2 + length;
        // 2 bytes:  packet type
        ofs += 2;
        packHeaderSize = (int)(ofs - startOfs);
        bool firstPack = ((fileBuffer->getByte(++ofs) & 0x80) == 0x80);
        // 1 byte    pts length
        int ptsLength = fileBuffer->getByte(ofs += 1);
        ofs += (1 + ptsLength); // skip PTS and stream ID
        int packetStreamID = fileBuffer->getByte(ofs++) & 0xf;
        if (packetStreamID != streamID)
        {
            // packet doesn't belong to stream -> skip
            if ((nextOfs % 0x800) != 0)
            {
                ofs = ((nextOfs / 0x800) + 1) * 0x800;
                //TODO: print error message
            }
            else
            {
                ofs = nextOfs;
            }
            ctrlOfs += 0x800;
            continue;
        }
        int headerSize = (int)(ofs - startOfs); // only valid for additional packets
        if (firstPack && ptsLength >= 5)
        {
            int size = fileBuffer->getWord(ofs);
            ofs += 2;
            ctrlOfsRel = fileBuffer->getWord(ofs);
            rleSize = ctrlOfsRel - 2;             // calculate size of RLE buffer
            ctrlSize = (size - ctrlOfsRel) - 2;       // calculate size of control header
            if (ctrlSize < 0)
            {
                //TODO: error handling
                throw 10;
            }
            ctrlHeader = QVector<uchar>(ctrlSize);
            ctrlOfs = ctrlOfsRel + ofs; // might have to be corrected for multiple packets
            ofs += 2;
            headerSize = (int)(ofs - startOfs);
            firstPackFound = true;
        }
        else
        {
            if (firstPackFound)
            {
                ctrlOfs += headerSize; // fix absolute offset by adding header bytes
            }
            else
            {
                //TODO: print warning message
            }
        }

        // check if control header is (partly) in this packet
        int diff = (int)((nextOfs - ctrlOfs) - ctrlHeaderCopied);
        if (diff < 0)
        {
            diff = 0;
        }
        int copied = ctrlHeaderCopied;
        try
        {
            for (int i = 0; (i < diff) && (ctrlHeaderCopied < ctrlSize); ++i)
            {
                ctrlHeader.replace(ctrlHeaderCopied, (uchar)fileBuffer->getByte(ctrlOfs + i + copied));
                ++ctrlHeaderCopied;
            }
        }
        catch (int e)
        {
            //TODO: Add error handling
            throw 10;
        }
        rleFrag = new ImageObjectFragment();
        rleFrag->imageBufferOfs = ofs;
        rleFrag->imagePacketSize = (((length - headerSize) - diff) + packHeaderSize);
        pic->rleFragments.push_back(rleFrag);

        rleBufferFound += rleFrag->imagePacketSize;

        if (ctrlHeaderCopied != ctrlSize && ((nextOfs % 0x800) != 0))
        {
            ofs = ((nextOfs / 0x800) + 1) * 0x800;
            //TODO: print error message
            rleBufferFound += ofs-nextOfs;
        }
        else
        {
            ofs = nextOfs;
        }
    } while ( ofs < endOfs && ctrlHeaderCopied < ctrlSize);

    if (ctrlHeaderCopied != ctrlSize)
    {
        //TODO: print warning to console
        for (int i = ctrlHeaderCopied; i < ctrlSize; ++i)
        {
            ctrlHeader.replace(i, 0xff);
        }
    }

    if (rleBufferFound != rleSize)
    {
        //TODO: print warning to console
    }

    pic->rleSize = rleBufferFound;
    int alphaSum = 0;
    QVector<int> alphaUpdate(4);
    int alphaUpdateSum;
    int delay = -1;
    bool ColAlphaUpdate = false;

    //TODO: print message to console

    int b;
    int index = 0;
    int endSeqOfs = (((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) - ctrlOfsRel) - 2;
    if (endSeqOfs < 0 || endSeqOfs > ctrlSize)
    {
        //TODO: print warning
        endSeqOfs = ctrlSize;
    }

    index += 2;
    while (index < endSeqOfs)
    {
        int cmd = ctrlHeader[index++] & 0xff;
        switch (cmd)
        {
        case 0: // forced (?)
        {
            pic->isForced = true;
            numForcedFrames++;
        } break;
        case 1: // start display
            break;
        case 3: // palette info
        {
            b = ctrlHeader[index++] & 0xff;
            pic->pal.replace(3, (b >> 4));
            pic->pal.replace(2, (b & 0x0f));
            b = ctrlHeader[index++] & 0xff;
            pic->pal.replace(1, (b >> 4));
            pic->pal.replace(0, (b & 0x0f));
            //TODO: print message
        } break;
        case 4: // alpha info
        {
            b = ctrlHeader[index++] & 0xff;
            pic->alpha.replace(3, (b >> 4));
            pic->alpha.replace(2, (b & 0x0f));
            b = ctrlHeader[index++] & 0xff;
            pic->alpha.replace(1, (b >> 4));
            pic->alpha.replace(0, (b & 0x0f));
            for (int i = 0; i<4; i++)
            {
                alphaSum += pic->alpha[i] & 0xff;
            }
            //TODO: print message
        } break;
        case 5: // coordinates
        {
            int xOfs = ((ctrlHeader[index] & 0xff) << 4) | ((ctrlHeader[index + 1] & 0xff) >> 4);
            pic->setOfsX(ofsXglob + xOfs);
            pic->setImageWidth((((((ctrlHeader[index + 1] & 0xff) & 0xf) << 8) | (ctrlHeader[index + 2] & 0xff)) - xOfs) + 1);
            int yOfs = ((ctrlHeader[index + 3] & 0xff) << 4) | ((ctrlHeader[index + 4] & 0xff) >> 4);
            pic->setOfsY(ofsYglob + yOfs);
            pic->setImageHeight((((((ctrlHeader[index + 4] & 0xff) & 0xf) << 8) | (ctrlHeader[index+5] & 0xff)) - yOfs) + 1);
            //TODO: print message
            index += 6;
        } break;
        case 6: // offset to RLE buffer
        {
            pic->evenOfs = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) - 4;
            pic->oddOfs = ((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - 4;
            index += 4;
            //TODO: print message
        } break;
        case 7: // color/alpha update
        {
            ColAlphaUpdate = true;
            // ignore the details for now, but just get alpha and palette info
            alphaUpdateSum = 0;
            b = ctrlHeader[index + 10] & 0xff;
            alphaUpdate.replace(3, (b >> 4));
            alphaUpdate.replace(2, (b & 0x0f));
            b = ctrlHeader[index + 11] & 0xff;
            alphaUpdate.replace(1, (b >> 4));
            alphaUpdate.replace(0, (b & 0x0f));
            for (int i = 0; i < 4; i++)
            {
                alphaUpdateSum += alphaUpdate[i] & 0xff;
            }
            // only use more opaque colors
            if (alphaUpdateSum > alphaSum) {
                alphaSum = alphaUpdateSum;
                for (int i = 0; i<4; i++)
                    pic->alpha.replace(i, alphaUpdate[i]);
                // take over frame palette
                b =ctrlHeader[index + 8] & 0xff;
                pic->pal.replace(3, (b >> 4));
                pic->pal.replace(2, (b & 0x0f));
                b = ctrlHeader[index + 9];
                pic->pal.replace(1, (b >> 4));
                pic->pal.replace(0, (b & 0x0f));
            }
            // search end sequence
            index = endSeqOfs;
            delay = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) *1024;
            endSeqOfs = (((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - ctrlOfsRel) - 2;
            if (endSeqOfs < 0 || endSeqOfs > ctrlSize)
            {
                //TODO: print warning
                endSeqOfs = ctrlSize;
            }
            index += 4;
        } break;
        case 0xff: // end sequence
             goto parse_ctrl;
        default:
            //TODO: print warning
            break;
        }
    }
    parse_ctrl:

    if (endSeqOfs != ctrlSize)
    {
        int ctrlSeqCount = 1;
        index = -1;
        int nextIndex = endSeqOfs;
        while (nextIndex != index)
        {
            index = nextIndex;
            delay = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) * 1024;
            nextIndex = (((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - ctrlOfsRel) - 2;
            ctrlSeqCount++;
        }
        if (ctrlSeqCount > 2)
        {
            //TODO: print warning
        }
        pic->endTime = pic->startTime + delay;
    }
    else
    {
        pic->endTime = pic->startTime;
    }

    if (ColAlphaUpdate)
    {
        //TODO: print warning
    }

    if (alphaSum == 0)
    {
        if (subtitleProcessor->getFixZeroAlpha())
        {
            for (int i=0; i < 4; i++)
            {
                pic->alpha.replace(i, lastAlpha[i]);
            }
            //TODO: print warning
        }
        else
        {
            //TODO: print warning
        }
    }

    lastAlpha = pic->alpha;

    pic->setOriginal();
}

void SubDVD::readAllSubFrames()
{
    for (int i = 0; i < subPictures.size(); ++i)
    {
        emit currentProgressChanged(i);
        //TODO: log progress in console
        long nextOfs;
        if (i < subPictures.size() - 1)
        {
            nextOfs = subPictures.at(i + 1)->offset;
        }
        else
        {
            nextOfs = fileBuffer->getSize();
        }
        readSubFrame(subPictures.at(i), nextOfs);
    }

    emit currentProgressChanged(subPictures.size());

    //TODO: print number of frames in console
}

void SubDVD::readIdx()
{
    if (!idxFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream idxTextStream(&idxFile);
    QString inString;
    int temp;
    int langIdx = 0;
    bool ignore = false;

    while (!(inString = idxTextStream.readLine()).isNull())
    {
        inString = inString.trimmed();
        if (inString.length() < 1 || inString[0] == '#')
        {
            continue;
        }
        int position = inString.indexOf(':');
        if (position == -1 || (inString.length() - position) <= 1)
        {
            //TODO: add error reporting
            continue;
        }
        QString key(inString.left(position).trimmed().toLower());
        QString value(inString.mid(position + 1, inString.length() - 1).trimmed());
        QStringList keyValue;

        // size (e.g. "size: 720x576")
        if (key == "size")
        {
            keyValue = value.split("x", QString::SkipEmptyParts);
            if (keyValue.isEmpty() || keyValue.size() <= 1)
            {
                //TODO: add error reporting
                return;
            }
            temp = keyValue[0].toInt();
            if (temp < 2)
            {
                //TODO: add error reporting
                return;
            }
            screenWidth = temp;
            temp = keyValue[1].toInt();
            if (temp < 2)
            {
                //TODO: add error reporting
                return;
            }
            screenHeight = temp;
            continue;
        }

        // origin (e.g. "org: 0, 0")
        if (key == "org")
        {
            keyValue = value.split(",", QString::SkipEmptyParts);
            if (keyValue.isEmpty() || keyValue.size() <= 1)
            {
                //TODO: add error reporting
                return;
            }
            temp = keyValue[0].toInt();
            if (temp < 0)
            {
                //TODO: add error reporting
                return;
            }
            ofsXglob = temp;
            temp =keyValue[1].toInt();
            if (temp < 0)
            {
                //TODO: add error reporting
                return;
            }
            ofsYglob = temp;
            continue;
        }

        // scale (e.g. "scale: 100%, 100%")
        if (key == "scale")
        {
            continue;
        }

        // alpha (e.g. "alpha: 100%")
        if (key == "alpha")
        {
            continue;
        }

        // smoothing (e.g. "smooth: OFF")
        if (key == "smooth")
        {
            continue;
        }

        // fading (e.g. "fadein/out: 0, 0");
        if (key == "fadein/out")
        {
            continue;
        }

        // alignment (e.g. "align: OFF at LEFT TOP")
        if (key == "align")
        {
            continue;
        }

        // time offset (e.g. "time offset: 0")
        if (key == "time offset")
        {
            temp = value.toInt();
            if (temp < 0)
            {
                temp = (int) TimeUtil::timeStrToPTS(value);
            }
            if (temp < 0)
            {
                //TODO: add error reporting
                return;
            }
            delayGlob = temp * 90; //ms -> 90khz
            continue;
        }

        // forced subs (e.g. "forced subs: OFF")
        if (key == "forced subs")
        {
            continue;
        }

        // palette (e.g. "palette: 000000, 828282, 828282, 828282, 828282, 828282, 828282, ffffff, 828282, bababa, 828282, 828282, 828282, 828282, 828282, 828282")
        if (key == "palette")
        {
            QStringList paletteValues = value.split(",");
            if (paletteValues.isEmpty() || paletteValues.length() < 1 || paletteValues.length() > 16)
            {
                //TODO: add error reporting
                return;
            }
            for (int i = 0; i < paletteValues.length(); ++i)
            {
                int color = -1;
                bool ok;
                temp = paletteValues[i].trimmed().toInt(&ok, 16);
                if (!ok)
                {
                    //TODO: add error reporting
                    return;
                }
                color = temp;
                srcPalette->setARGB(i, color);
            }
            continue;
        }

        // custom colors (e.g. "custom colors: OFF, tridx: 1000, colors: 000000, 444444, 888888, cccccc")
        if (key == "custom colors")
        {
            continue;
        }

        // language index (e.g. "langidx: 0")
        if (key == "langidx")
        {
            temp = value.toInt();
            if (temp < 0)
            {
                //TODO: add error reporting
                return;
            }
            langIdx = temp;
            continue;
        }

        // language id (e.g. "id: de, index: 0")
        if (key == "id")
        {
            QString id;
            QStringList vals = value.split(',', QString::SkipEmptyParts);
            id = vals[0];
            if (id.length() != 2)
            {
                //TODO: add error reporting
                continue;
            }
            bool found = false;
            auto subLanguages = subtitleProcessor->getLanguages();

            for (int i = 0; i < subLanguages.size(); ++i)
            {
                if (id == subLanguages[i][1])
                {
                    languageIdx = i;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                //TODO: add error reporting
            }
            vals = value.split(':', QString::SkipEmptyParts);
            if (vals[0].toLower() == "index")
            {
                //TODO: add error reporting
                continue;
            }
            temp = vals[1].toInt();
            if (temp < 0)
            {
                //TODO: add error reporting
                return;
            }
            if (temp != langIdx)
            {
                ignore = true;
                //TODO: add error reporting
            }
            else
            {
                streamID = temp;
                ignore = false;
            }
            continue;
        }

        if (!ignore)
        {
            // e.g. ("timestamp: 00:00:14:160, filepos: 000000000")
            if (key == "timestamp")
            {
                QString vs;
                QStringList vals = value.split(',', QString::SkipEmptyParts);
                if (vals.isEmpty() || vals.size() <= 1)
                {
                    //TODO: add error reporting
                    return;
                }
                vs = vals[0];
                long time = TimeUtil::timeStrToPTS(vs);
                if (time < 0)
                {
                    //TODO: add error reporting
                    return;
                }
                vs = vals[1].trimmed().toLower();
                vals = vs.split(':', QString::SkipEmptyParts);
                if (vals.isEmpty() || vals.size() <= 1)
                {
                    //TODO: add error reporting
                    return;
                }
                bool ok;
                long hex = vals[1].trimmed().toLong(&ok, 16);
                if (!ok)
                {
                    //TODO: add error reporting
                    return;
                }
                SubPictureDVD* pic = new SubPictureDVD;
                pic->offset = hex;
                pic->width = screenWidth;
                pic->height = screenHeight;
                pic->startTime = time + delayGlob;
                subPictures.push_back(pic);
            }
        }
    }

    idxFile.close();

    emit maxProgressChanged(subPictures.size());
}
