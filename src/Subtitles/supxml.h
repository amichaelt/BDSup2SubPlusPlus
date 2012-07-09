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

#include "bitmap.h"
#include "substream.h"

#include <QObject>
#include <QFile>
#include <QtXml/QXmlDefaultHandler>
#include <QStringList>
#include <QString>
#include <QVector>
#include <QScopedPointer>

class SubtitleProcessor;
class SubPictureXML;
class QImage;
class BitStream;
class XmlHandler;

enum class Resolution : int;

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

        bool valid = false;

        QStringList xmlStates = { "bdn", "description", "name", "language", "format", "events", "event", "graphic" };

        QString txt;

        QVector<int> getResolutions(Resolution resolution);

        Resolution getResolution (QString string);

        SubPictureXML* subPicture;

        SupXML* parent;

        enum class XmlState { BDN, DESCRIPT, NAME, LANGUAGE, FORMAT, EVENTS, EVENT, GRAPHIC, UNKNOWN };

        XmlState state;
        XmlState findState(QString string);
    };

public:
    SupXML(QString fileName, SubtitleProcessor* subtitleProcessor);
    ~SupXML();

    void decode(int index);
    void readAllImages();
    void writeXml(QString filename, QVector<SubPicture*> pics);

    int getPrimaryColorIndex() { return primaryColorIndex; }
    int getNumFrames();
    int getNumForcedFrames() { return numForcedFrames; }

    long getEndTime(int index);
    long getStartTime(int index);
    long getStartOffset(int index) { return 0; }

    double getFps() { return fps; }

    bool isForced(int index);

    Bitmap &getBitmap() { return bitmap; }

    Palette *getPalette() { return palette.data(); }

    QImage getImage();
    QImage getImage(Bitmap *bitmap);

    QString getLanguage() { return language; }
    QString getPNGname(QString filename, int idx);

    SubPicture *getSubPicture(int index);

signals:
    void maxProgressChanged(int maxProgress);
    void currentProgressChanged(int currentProgress);

private:
    int primaryColorIndex = 0;
    int numToImport = 0;
    double fps;
    double fpsXml;

    Bitmap bitmap;

    QScopedPointer<Palette> palette;

    QScopedPointer<QFile> xmlFile;

    QString title = "";
    QString pathName;
    QString language = "deu";
    QString xmlFileName;

    QVector<SubPictureXML*> subPictures;

    Resolution resolution;

    SubtitleProcessor* subtitleProcessor = 0;

    double XmlFps(double fps);
};

#endif // SUPXML_H
