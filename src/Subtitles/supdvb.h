#ifndef SUPDVB_H
#define SUPDVB_H

#include "substream.h"
#include "bitmap.h"
#include "palette.h"

#include <QObject>
#include <QScopedPointer>

#define PAGE_COMPOSITION_SEGMENT 0x10
#define REGION_COMPOSITION_SEGMENT 0x11
#define CLUT_DEFINITION_SEGMENT 0x12
#define OBJECT_DATA_SEGMENT 0x13
#define DISPLAY_DEFINITION_SEGMENT 0x14
#define END_OF_DISPLAY_SET_SEGMENT 0x80

#define SYNC_BYTE 0x0F

class Palette;
class SubtitleProcessor;
class FileBuffer;

class SupDVB : public QObject, public Substream
{
    Q_OBJECT
public:
    SupDVB(QString supFileName, SubtitleProcessor* subtitleProcessor);
    ~SupDVB();

    void decode(int index) { }
    void readAllSupFrames();

    int getPrimaryColorIndex() { }
    int getNumFrames() { }
    int getNumForcedFrames() { }

    long getEndTime(int index) { }
    long getStartTime(int index) { }
    long getStartOffset(int index) { }

    bool isForced(int index) { }

    Bitmap &getBitmap() { }

    Palette &getPalette() { }

    SubPicture *getSubPicture(int index) { }

    QImage getImage() { }
    QImage getImage(Bitmap &bitmap) { }

signals:
    void maxProgressChanged(long maxProgress);
    void currentProgressChanged(long currentProgress);

private:

    Bitmap bitmap;

    Palette palette;

    QScopedPointer<FileBuffer> fileBuffer;

    QString supFileName;

    QVector<SubPictureDVD*> subPictures;

    SubtitleProcessor* subtitleProcessor = 0;

    long readSupFrame(long ofs);
};

#endif // SUPDVB_H
