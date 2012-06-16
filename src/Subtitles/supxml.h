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

#ifndef SUPXML_H
#define SUPXML_H

#include "substream.h"
#include "types.h"
#include <QObject>
#include <QFile>
#include <QtXml/QXmlDefaultHandler>
#include <QStringList>
#include <QString>

class SubtitleProcessor;
class SubPictureXML;
class QImage;
class BitStream;
class XmlHandler;

class SupXML : public QObject, public Substream
{
    Q_OBJECT

    class XmlHandler : public QXmlDefaultHandler
    {
    public:
        XmlHandler(SupXML* parent) { this->parent = parent; }
        bool characters(const QString &ch);
        bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
        bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);

    private:
        enum class XmlState { BDN, DESCRIPT, NAME, LANGUAGE, FORMAT, EVENTS, EVENT, GRAPHIC, UNKNOWN };
        QStringList xmlStates = { "bdn", "description", "name", "language", "format", "events", "event", "graphic" };

        XmlState state;
        QString txt;
        bool valid = false;
        SubPictureXML* subPicture;
        SupXML* parent;

        XmlState findState(QString string);
        Resolution getResolution (QString string);
        QVector<int> getResolutions(Resolution resolution);
    };

public:
    SupXML(QString fileName, SubtitleProcessor* subtitleProcessor);
    ~SupXML();

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
    long getStartOffset(int index) { return 0; }
    SubPicture *getSubPicture(int index);

    void readAllImages();
    QString getLanguage() { return language; }
    double getFps() { return fps; }
    QString getPNGname(QString filename, int idx);
    void writeXml(QString filename, QVector<SubPicture*> pics);

signals:
    void maxProgressChanged(int maxProgress);
    void currentProgressChanged(int currentProgress);

private:
    QString xmlFileName;
    QScopedPointer<QFile> xmlFile;
    QString title = "";
    QString pathName;
    QString language = "deu";
    Resolution resolution = Resolution::HD_1080;
    QVector<SubPictureXML*> subPictures;
    FileBuffer *fileBuffer = 0;
    Palette *palette = 0;
    Bitmap *bitmap = 0;
    int primaryColorIndex = 0;
    int numToImport = 0;
    double fps = FPS_24P;
    double fpsXml = FPS_24P;
    SubtitleProcessor* subtitleProcessor = 0;

    double XmlFps(double fps);
};

#endif // SUPXML_H
