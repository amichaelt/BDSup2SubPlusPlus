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

#include "supxml.h"
#include "subtitleprocessor.h"
#include "bitmap.h"
#include "palette.h"
#include "subpicturexml.h"
#include "Tools/timeutil.h"
#include "Tools/quantizefilter.h"
#include "types.h"
#include <QImage>
#include <QFileInfo>
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlInputSource>
#include <QRect>
#include <QDir>
#include <QPainter>

SupXML::SupXML(QString fileName, SubtitleProcessor* subtitleProcessor) :
    fps(FPS_24P),
    fpsXml(FPS_24P),
    xmlFileName(fileName),
    resolution(Resolution::HD_1080)
{
    pathName = QFileInfo(fileName).absolutePath() + QDir::separator();
    this->subtitleProcessor = subtitleProcessor;
    fpsXml = XmlFps(fps);
    xmlFile.reset(new QFile(xmlFileName));
}

SupXML::~SupXML()
{
    if (!xmlFile.isNull())
    {
        xmlFile.reset();
    }
}

QImage SupXML::image()
{
    return _bitmap.image(_palette);
}

QImage SupXML::image(Bitmap &bitmap)
{
    return bitmap.image(_palette);
}

void SupXML::decode(int index)
{
    QVector<Bitmap> bitmaps;
    QVector<Palette> palettes;
    SubPictureXML subPic = subPictures[index];
    for (int i = 0; i < subPic.fileNames().size(); ++i)
    {
        if (!QFileInfo(subPic.fileNames()[i]).exists())
        {
            throw QString("File: '%1' not found").arg(subPic.fileNames()[i]);
        }

        Bitmap bitmap;
        Palette palette = Palette(0, true);
        QImage image(subPic.fileNames()[i]);
        int width = image.width();
        int height = image.height();

        // first try to read image and palette directly from imported image
        if (image.format() == QImage::Format_Indexed8)
        {
            QVector<QRgb> colorTable = image.colorTable();
            if (colorTable.size() <= 255 || (image.hasAlphaChannel() && qAlpha(colorTable[255]) == 0))
            {
                // create palette
                palette = Palette(256);
                for (int i = 0; i < colorTable.size(); ++i)
                {
                    int alpha = (colorTable[i] >> 24) & 0xff;
                    if (alpha >= subtitleProcessor->getAlphaCrop())
                    {
                        palette.setARGB(i, colorTable[i]);
                    }
                    else
                    {
                        palette.setARGB(i, 0);
                    }
                }
                bitmap = Bitmap(image);
            }
        }

        // if this failed, assume RGB image and quantize palette
        if (palette.size() == 0)
        {
            // quantize image
            QuantizeFilter qf;
            bitmap = Bitmap(image.width(), image.height());
            QVector<QRgb> ct = qf.quantize(image, &bitmap.image(), width, height, 255, false, false);
            int size = ct.size();
            if (size > 255)
            {
                subtitleProcessor->printWarning("Quantizer failed.\n");
                size = 255;
            }
            // create palette
            palette = Palette(256);
            for (int i = 0; i < size; ++i)
            {
                int alpha = qAlpha(ct[i]);
                if (alpha >= subtitleProcessor->getAlphaCrop())
                {
                    palette.setARGB(i, ct[i]);
                }
                else
                {
                    palette.setARGB(i, 0);
                }
            }
        }
        bitmaps.push_back(bitmap);
        palettes.push_back(palette);
    }

    //combine images
    if (bitmaps.size() == 1)
    {
        _bitmap = bitmaps[0];
        _palette = palettes[0];
    }
    else
    {
        QImage resultImage(1920, 1080, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&resultImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(resultImage.rect(), Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(subPic.windowSizes()[0].x(), subPic.windowSizes()[0].y(), bitmaps[0].image(palettes[0]));
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(subPic.windowSizes()[1].x(), subPic.windowSizes()[1].y(), bitmaps[1].image(palettes[1]));
        painter.end();

        resultImage = resultImage.convertToFormat(QImage::Format_Indexed8, palettes[0].colorTable());

        _bitmap = Bitmap(resultImage);
        _palette = palettes[0];

        if (subPic.windowSizes()[0].y() > subPic.windowSizes()[1].y())
        {
            subPictures[index].setImageHeight(subPic.windowSizes()[0].y() + subPic.windowSizes()[0].height());
        }
        else
        {
            subPictures[index].setImageHeight(subPic.windowSizes()[1].y() + subPic.windowSizes()[1].height());
        }
        subPic.setImageHeight(subPictures[index].imageHeight());
    }

    _primaryColorIndex = _bitmap.primaryColorIndex(_palette, subtitleProcessor->getAlphaThreshold());
    // crop
    QRect bounds = _bitmap.bounds(_palette, subtitleProcessor->getAlphaCrop());
    if (bounds.topLeft().y() > 0 || bounds.topLeft().x() > 0 ||
        bounds.bottomRight().x() < (_bitmap.width() - 1) ||
        bounds.bottomRight().y() < (_bitmap.height() - 1))
    {
        int width = bounds.width();
        int height = bounds.height();

        if (width < 2)
        {
            width = 2;
        }
        if (height < 2)
        {
            height = 2;
        }
        _bitmap = _bitmap.crop(bounds.topLeft().x(), bounds.topLeft().y(), width, height);
        // update picture
        SubPictureXML pic(subPic);
        pic.setImageWidth(width);
        pic.setImageHeight(height);
        pic.setX(pic.originalX() + bounds.topLeft().x());
        pic.setY(pic.originalY() + bounds.topLeft().y());
    }
}

int SupXML::numFrames()
{
    return subPictures.size();
}

bool SupXML::isForced(int index)
{
    return subPictures[index].isForced();
}

qint64 SupXML::endTime(int index)
{
    return subPictures[index].endTime();
}

qint64 SupXML::startTime(int index)
{
    return subPictures[index].startTime();
}

SubPicture *SupXML::subPicture(int index)
{
    return &subPictures[index];
}

void SupXML::readAllImages()
{
    QXmlSimpleReader xmlReader;
    QXmlInputSource *source = new QXmlInputSource(xmlFile.data());

    XmlHandler *handler = new XmlHandler(this);
    xmlReader.setContentHandler(handler);
    xmlReader.setErrorHandler(handler);

    if (!xmlReader.parse(source))
    {
        throw QString("Failed to parse file: '%1'").arg(xmlFileName);
    }

    subtitleProcessor->print(QString("\nDetected %1 forced captions.\n").arg(QString::number(_numForcedFrames)));
}

QString SupXML::getPNGname(QString filename, int idx)
{
    QFileInfo info(filename);
    return QString("%1/%2_%3.png").arg(info.absolutePath()).arg(info.completeBaseName()).arg(QString::number(idx), 4, QChar('0'));
}

void SupXML::writeXml(QString filename, QVector<SubPicture*> pics)
{
    double fps = subtitleProcessor->getFPSTrg();
    double fpsXml = XmlFps(fps);
    qint64 t;
    QScopedPointer<QFile> out(new QFile(filename));
    QString name = QFileInfo(filename).completeBaseName();
    if (!out->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw QString("File: '%1' can not be opened for reading.").arg(filename);
    }
    out->write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    out->write("<BDN Version=\"0.93\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"BD-03-006-0093b BDN File Format.xsd\">\n");
    out->write("  <Description>\n");
    out->write(QString("    <Name Title=\"" + name + "\" Content=\"\"/>\n").toLatin1());
    out->write(QString("    <Language Code=\"" + subtitleProcessor->getLanguages()[subtitleProcessor->getLanguageIdx()][2] + "\"/>\n").toLatin1());
    QString res = subtitleProcessor->getResolutionNameXml((int)subtitleProcessor->getOutputResolution());
    out->write(QString("    <Format VideoFormat=\"" + res + "\" FrameRate=\"" + QString::number(fps, 'g', 6) + "\" DropFrame=\"False\"/>\n").toLatin1());
    t = pics[0]->startTime();
    if (fps != fpsXml)
    {
        t = ((t * 2000) + 1001) / 2002;
    }
    QString ts = TimeUtil::ptsToTimeStrXml(t, fpsXml);
    t = pics[pics.size() - 1]->endTime();
    if (fps != fpsXml)
    {
        t = ((t * 2000) + 1001) / 2002;
    }
    QString te = TimeUtil::ptsToTimeStrXml(t, fpsXml);
    out->write(QString("    <Events Type=\"Graphic\" FirstEventInTC=\"" + ts + "\" LastEventOutTC=\"" + te + "\" NumberofEvents=\"" + QString::number(pics.size()) + "\"/>\n").toLatin1());
    out->write("  </Description>\n");
    out->write("  <Events>\n");
    for (int idx = 0; idx < pics.size(); ++idx)
    {
        SubPicture* p = pics[idx];
        t = p->startTime();
        if (fps != fpsXml)
        {
            t = ((t * 2000) + 1001) / 2002;
        }
        ts = TimeUtil::ptsToTimeStrXml(t, fpsXml);
        t = p->endTime();
        if (fps != fpsXml)
        {
            t = ((t * 2000) + 1001) / 2002;
        }
        te = TimeUtil::ptsToTimeStrXml(t, fpsXml);
        QString forced = p->isForced() ? "True": "False";
        out->write(QString("    <Event InTC=\"" + ts + "\" OutTC=\"" + te + "\" Forced=\"" + forced + "\">\n").toLatin1());

        QString pname = QFileInfo(getPNGname(name, idx + 1)).fileName();
        out->write(QString("      <Graphic Width=\"" + QString::number(p->imageWidth()) + "\" Height=\"" + QString::number(p->imageHeight()) +
                          "\" X=\"" + QString::number(p->x()) + "\" Y=\"" + QString::number(p->y()) + "\">" + pname + "</Graphic>\n").toLatin1());
        out->write("    </Event>\n");
    }
    out->write("  </Events>\n");
    out->write("</BDN>\n");
}

double SupXML::XmlFps(double fps)
{
    if (fps == FPS_23_975)
    {
        return FPS_24HZ;
    }
    if (fps == FPS_24P)
    {
        return FPS_24HZ;
    }
    if (fps == FPS_NTSC)
    {
        return 30.0;
    }
    if (fps == FPS_NTSC_I)
    {
        return 60.0;
    }
    return fps;
}


bool SupXML::XmlHandler::characters(const QString &ch)
{
    txt += ch;
    return true;
}


bool SupXML::XmlHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    XmlState endState = findState(qName.toLower());
    if (state == XmlState::GRAPHIC && endState == XmlState::GRAPHIC)
    {
        subPicture->setFileName(parent->pathName + txt.trimmed());
    }
    return true;
}

bool SupXML::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                      const QString &qName, const QXmlAttributes &atts)
{
    state = findState(qName.toLower());
    QString at;

    if (state != SupXML::XmlHandler::XmlState::BDN && !valid)
    {
        parent->subtitleProcessor->printError("BDN tag missing\n");
    }

    txt = QString("");

    switch ((int)state)
    {
    case (int)SupXML::XmlHandler::XmlState::UNKNOWN:
    {
         parent->subtitleProcessor->printError(QString("Unknown tag %1\n").arg(qName));
    } break;
    case (int)SupXML::XmlHandler::XmlState::BDN:
    {
        if (valid)
        {
            parent->subtitleProcessor->printError("BDN must be used only once\n");
        }
        else
        {
            valid = true;
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::NAME:
    {
        at = atts.value("Title");
        if (!at.isEmpty())
        {
            parent->title = at;
            parent->subtitleProcessor->print(QString("Title: %1\n").arg(parent->title));
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::LANGUAGE:
    {
        at = atts.value("Code");
        if (!at.isEmpty())
        {
            parent->language = at;
            parent->subtitleProcessor->print(QString("Language: %1\n").arg(parent->language));
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::FORMAT:
    {
        at = atts.value("FrameRate");
        if (!at.isEmpty())
        {
            parent->fps = parent->subtitleProcessor->getFPS(at);
            parent->fpsXml = parent->XmlFps(parent->fps);
            parent->subtitleProcessor->print(QString("fps: %1\n").arg(QString::number(parent->fps, 'g', 6)));
        }
        at = atts.value("VideoFormat");
        if (!at.isEmpty())
        {
            QString res = QString(at);
            if (res.length() == 4 && res[0] != '7')
            {
                res = res.replace('p', 'i');
            }
            parent->resolution = parent->subtitleProcessor->getResolution(res);
            parent->subtitleProcessor->print(QString("Language: %1\n").arg(res));
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::EVENTS:
    {
        at = atts.value("NumberofEvents");
        if (!at.isEmpty())
        {
            bool ok;
            int n = at.toInt(&ok);
            n = ok ? n : -1;
            if (n > 0)
            {
                parent->numToImport = n;
                emit parent->maxProgressChanged(parent->numToImport);
            }
            else
            {
                throw QString("The NumberofEvents in the XML file is invalid.");
            }
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::EVENT:
    {
        parent->subPictures.push_back(SubPictureXML());
        int num  = parent->subPictures.size();
        subPicture = &parent->subPictures[num - 1];

        parent->subtitleProcessor->printX(QString("#%1\n").arg(QString::number(num)));

        emit parent->currentProgressChanged(num);
        at = atts.value("InTC");

        if (!at.isEmpty())
        {
            subPicture->setStartTime(TimeUtil::timeStrXmlToPTS(at, parent->fpsXml));
            if (subPicture->startTime() == -1)
            {
                subPicture->setStartTime(0);
                parent->subtitleProcessor->printWarning(QString("Invalid start time %1\n").arg(at));
            }
        }
        at = atts.value("OutTC");
        if (!at.isEmpty())
        {
            subPicture->setEndTime(TimeUtil::timeStrXmlToPTS(at, parent->fpsXml));
            if (subPicture->endTime() == -1)
            {
                subPicture->setEndTime(0);
                parent->subtitleProcessor->printWarning(QString("Invalid end time %1\n").arg(at));
            }
        }
        if (parent->fps != parent->fpsXml)
        {
            subPicture->setStartTime(((subPicture->startTime() * 1001) + 500) / 1000);
            subPicture->setEndTime(((subPicture->endTime() * 1001) + 500) / 1000);
        }
        at = atts.value("Forced");
        if (!at.isEmpty())
        {
            subPicture->setForced(at.toLower() == "true");
        }
        else
        {
            subPicture->setForced(false);
        }
        if (subPicture->isForced())
        {
            parent->_numForcedFrames++;
        }
        QVector<int> dim = parent->subtitleProcessor->getResolutions(parent->resolution);
        subPicture->setScreenWidth(dim.at(0));
        subPicture->setScreenHeight(dim.at(1));
    } break;
    case (int)SupXML::XmlHandler::XmlState::GRAPHIC:
    {
        bool hasGraphic = subPicture->fileNames().size() == 1;
        bool ok;
        int width = atts.value("Width").toInt(&ok);
        width = ok ? width : -1;
        if (!hasGraphic || width > subPicture->imageWidth())
        {
            subPicture->setImageWidth(width);
        }

        int height = atts.value("Height").toInt(&ok);
        height = ok ? height : -1;
        if (!hasGraphic || height > subPicture->imageHeight())
        {
            subPicture->setImageHeight(height);
        }

        int x = atts.value("X").toInt(&ok);
        x = ok ? x : -1;
        if (!hasGraphic || x < subPicture->x())
        {
            subPicture->setX(x);
        }

        int y = atts.value("Y").toInt(&ok);
        y = ok ? y : -1;
        if (!hasGraphic || y < subPicture->y())
        {
            subPicture->setY(y);
        }
        subPicture->setOriginal();

        QRect rect(x, y, width, height);
        subPicture->imageRects.push_back(rect);
        subPicture->scaledImageRects.push_back(rect);
    } break;
    }

    return true;
}

SupXML::XmlHandler::XmlState SupXML::XmlHandler::findState(QString string)
{
    if (string == "bdn")
    {
        return SupXML::XmlHandler::XmlState::BDN;
    }
    if (string == "description")
    {
        return SupXML::XmlHandler::XmlState::DESCRIPT;
    }
    if (string == "name")
    {
        return SupXML::XmlHandler::XmlState::NAME;
    }
    if (string == "language")
    {
        return SupXML::XmlHandler::XmlState::LANGUAGE;
    }
    if (string == "format")
    {
        return SupXML::XmlHandler::XmlState::FORMAT;
    }
    if (string == "events")
    {
        return SupXML::XmlHandler::XmlState::EVENTS;
    }
    if (string == "event")
    {
        return SupXML::XmlHandler::XmlState::EVENT;
    }
    if (string == "graphic")
    {
        return SupXML::XmlHandler::XmlState::GRAPHIC;
    }
    return SupXML::XmlHandler::XmlState::UNKNOWN;
}
