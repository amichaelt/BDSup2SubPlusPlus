#ifndef SUPHD_H
#define SUPHD_H

#include "substream.h"
#include <QObject>
#include <QString>

class SubtitleProcessor;

class SupHD : public QObject, public Substream
{
    Q_OBJECT

public:
    SupHD(QString fileName, SubtitleProcessor* subtitleProcessor);

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

    void readAllSupFrames();

signals:
    void maxProgressChanged(int maxProgress);
    void currentProgressChanged(int currentProgress);

private:
    QString supFileName;
};

#endif // SUPHD_H
