#include "supxml.h"
#include "subtitleprocessor.h"
#include "bitmap.h"
#include "palette.h"
#include "subpicturexml.h"
#include "Tools/timeutil.h"
#include <QImage>
#include <QFileInfo>
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlInputSource>
#include <QRect>

SupXML::SupXML(QString fileName, SubtitleProcessor* subtitleProcessor) :
    xmlFileName(fileName),
    xmlFile(xmlFileName)
{
    pathName = QFileInfo(fileName).absolutePath() + "/";
    this->subtitleProcessor = subtitleProcessor;
    fpsXml = XmlFps(fps);
}

Palette *SupXML::getPalette()
{
    return palette;
}

Bitmap *SupXML::getBitmap()
{
    return bitmap;
}

QImage *SupXML::getImage()
{
    return bitmap->getImage(palette);
}

QImage *SupXML::getImage(Bitmap *bitmap)
{
    return bitmap->getImage(palette);
}

int SupXML::getPrimaryColorIndex()
{
    return primaryColorIndex;
}

void SupXML::decode(int index)
{
    QFile imageFile(subPictures.at(index)->fileName);
    if (!imageFile.exists())
    {
        //TODO: error handling
        throw 10;
    }
    QImage* image = new QImage(subPictures.at(index)->fileName);
    int width = image->width();
    int height = image->height();

    palette = 0;

    // first try to read image and palette directly from imported image
    if (image->format() == QImage::Format_Indexed8)
    {
        QVector<QRgb> colorTable = image->colorTable();
        if (colorTable.size() < 255 || (image->hasAlphaChannel() && qAlpha(colorTable[255]) == 0))
        {
            // create palette
            palette = new Palette(256);
            for (int i = 0; i < colorTable.size(); ++i)
            {
                int alpha = (colorTable[i] >> 24) & 0xff;
                if (alpha >= subtitleProcessor->getAlphaCrop())
                {
                    palette->setARGB(i, colorTable[i]);
                }
                else
                {
                    palette->setARGB(i, 0);
                }
            }
            bitmap = new Bitmap(image->width(), image->height(), image);
        }
    }

    // if this failed, assume RGB image and quantize palette
    if (palette == 0)
    {
//        // grab int array (ARGB)
//        QVector<int> pixels(image->bytesPerLine() * height);
//        image->getRGB(0, 0, width, height, pixels, 0, width);
//        // quantize image
//        QuantizeFilter qf = new QuantizeFilter();
//        bitmap = new Bitmap(image.getWidth(), image.getHeight());
//        int ct[] = qf.quantize(pixels, bitmap.getImg(), width, height, 255, false, false);
//        int size = ct.length;
//        if (size > 255)
//        {
//            //TODO: print warning
//            size = 255;
//        }
//        // create palette
//        palette = new Palette(256);
//        for (int i = 0; i < size; ++i)
//        {
//            int alpha = (ct[i] >> 24) & 0xff;
//            if (alpha >= subtitleProcessor->getAlphaCrop())
//            {
//                palette.setARGB(i, ct[i]);
//            }
//            else
//            {
//                palette.setARGB(i, 0);
//            }
//        }
    }

    primaryColorIndex = bitmap->getPrimaryColorIndex(palette, subtitleProcessor->getAlphaThreshold());
    // crop
    QRect bounds = bitmap->getBounds(palette, subtitleProcessor->getAlphaCrop());
    if (bounds.topLeft().y() > 0 || bounds.topLeft().x() > 0 ||
        bounds.bottomRight().x() < (bitmap->getWidth() - 1) ||
        bounds.bottomRight().y() < (bitmap->getHeight() - 1))
    {
        width = bounds.width();
        height = bounds.height();

        if (width < 2)
        {
            width = 2;
        }
        if (height < 2)
        {
            height = 2;
        }
        bitmap = bitmap->crop(bounds.topLeft().x(), bounds.topLeft().y(), width, height);
        // update picture
        SubPictureXML* pic = subPictures.at(index);
        pic->setImageWidth(width);
        pic->setImageHeight(height);
        pic->setOfsX(pic->originalX + bounds.topLeft().x());
        pic->setOfsY(pic->originalY + bounds.topLeft().y());
    }
}

int SupXML::getNumFrames()
{
    return subPictures.size();
}

int SupXML::getNumForcedFrames()
{
    return numForcedFrames;
}

bool SupXML::isForced(int index)
{
    return subPictures.at(index)->isForced;
}

void SupXML::close()
{
    if (xmlFile.isOpen())
    {
        xmlFile.close();
    }
}

long SupXML::getEndTime(int index)
{
    return subPictures.at(index)->endTime;
}

long SupXML::getStartTime(int index)
{
    return subPictures.at(index)->startTime;
}

long SupXML::getStartOffset(int index)
{
    return 0;
}

SubPicture *SupXML::getSubPicture(int index)
{
    return subPictures.at(index);
}

void SupXML::readAllImages()
{
    QXmlSimpleReader xmlReader;
    QXmlInputSource *source = new QXmlInputSource(&xmlFile);

    XmlHandler *handler = new XmlHandler(this);
    xmlReader.setContentHandler(handler);
    xmlReader.setErrorHandler(handler);

    if (!xmlReader.parse(source))
    {
        //TODO: error handling
        throw 10;
    }
    //TODO: print message;
    int num = numForcedFrames;
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
        subPicture->fileName = parent->pathName + txt.trimmed();
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
        //TODO: print error;
    }

    txt = QString("");

    switch ((int)state)
    {
    case (int)SupXML::XmlHandler::XmlState::UNKNOWN:
    {
        //TODO: print error
    } break;
    case (int)SupXML::XmlHandler::XmlState::BDN:
    {
        if (valid)
        {
            //TODO: print error
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
            //TODO: print message
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::LANGUAGE:
    {
        at = atts.value("Code");
        if (!at.isEmpty())
        {
            parent->language = at;
            //TODO: print message
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::FORMAT:
    {
        at = atts.value("FrameRate");
        if (!at.isEmpty())
        {
            parent->fps = parent->subtitleProcessor->getFPS(at);
            parent->fpsXml = parent->XmlFps(parent->fps);
            //TODO: print message
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
            //TODO: print message
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::EVENTS:
    {
        at = atts.value("NumberofEvents");
        if (!at.isEmpty())
        {
            bool ok;
            int n = at.toInt(&ok);
            if (!ok)
            {
                n = -1;
            }
            if (n > 0)
            {
                parent->numToImport = n;
                emit parent->maxProgressChanged(parent->numToImport);
            }
        }
    } break;
    case (int)SupXML::XmlHandler::XmlState::EVENT:
    {
        subPicture = new SubPictureXML();
        parent->subPictures.push_back(subPicture);
        int num  = parent->subPictures.size();
        //TODO: print message
        emit parent->currentProgressChanged(num);
        at = atts.value("InTC");
        if (!at.isEmpty())
        {
            subPicture->startTime = TimeUtil::timeStrXmlToPTS(at, parent->fpsXml);
            if (subPicture->startTime == -1)
            {
                subPicture->startTime = 0;
                //TODO: print warning
            }
        }
        at = atts.value("OutTC");
        if (!at.isEmpty())
        {
            subPicture->endTime = TimeUtil::timeStrXmlToPTS(at, parent->fpsXml);
            if (subPicture->endTime == -1)
            {
                subPicture->endTime = 0;
                //TODO: print warning
            }
        }
        if (parent->fps != parent->fpsXml)
        {
            subPicture->startTime = ((subPicture->startTime * 1001) + 500) / 1000;
            subPicture->endTime   = ((subPicture->endTime * 1001) + 500) / 1000;
        }
        at = atts.value("Forced");
        if (!at.isEmpty())
        {
            subPicture->isForced  = at.toLower() == "true";
        }
        else
        {
            subPicture->isForced = false;
        }
        if (subPicture->isForced)
        {
            parent->numForcedFrames++;
        }
        QVector<int> dim = parent->subtitleProcessor->getResolutions(parent->resolution);
        subPicture->width  = dim.at(0);
        subPicture->height = dim.at(1);
    } break;
    case (int)SupXML::XmlHandler::XmlState::GRAPHIC:
    {
        bool ok;
        int width = atts.value("Width").toInt(&ok);
        if (!ok)
        {
            width = -1;
        }
        subPicture->setImageWidth(width);

        int height = atts.value("Height").toInt(&ok);
        if (!ok)
        {
            height = -1;
        }
        subPicture->setImageHeight(height);

        int x = atts.value("X").toInt(&ok);
        if (!ok)
        {
            x = -1;
        }
        subPicture->setOfsX(x);

        int y = atts.value("Y").toInt(&ok);
        if (!ok)
        {
            y = -1;
        }
        subPicture->setOfsY(y);
        subPicture->setOriginal();
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
