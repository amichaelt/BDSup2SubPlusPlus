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

#ifndef SUPBD_H
#define SUPBD_H

#include "substream.h"
#include "bitmap.h"
#include "palette.h"

#include <QObject>
#include <QString>
#include <QScopedPointer>
#include <QVector>

class SubtitleProcessor;
class SubPictureBD;
class FileBuffer;

class SupBD : public QObject, public Substream
{
    Q_OBJECT

public:
    SupBD(QString fileName, SubtitleProcessor* subtitleProcessor);
    ~SupBD();

    void decode(int index);
    void readAllSupFrames();

    int getPrimaryColorIndex() { return primaryColorIndex; }
    int getNumFrames();
    int getNumForcedFrames() { return numForcedFrames; }

    long getEndTime(int index);
    long getStartTime(int index);
    long getStartOffset(int index);

    double getFps(int index);

    bool isForced(int index);

    Bitmap &getBitmap() { return bitmap; }

    Palette &getPalette() { return palette; }

    QImage getImage();
    QImage getImage(Bitmap &bitmap);

    SubPicture *getSubPicture(int index);

    QVector<uchar> createSupFrame(SubPicture &subPicture, Bitmap &bm, Palette &pal);

signals:
    void maxProgressChanged(int maxProgress);
    void currentProgressChanged(int currentProgress);

private:
    struct SupSegment
    {
        int type = 0;
        int size = 0;
        long timestamp = 0;
        int offset = 0;
    };

    enum class CompositionState
    {
        /** normal: doesn't have to be complete */
        NORMAL,
        /** acquisition point */
        ACQU_POINT,
        /** epoch start - clears the screen */
        EPOCH_START,
        /** epoch continue */
        EPOCH_CONTINUE,
        /** unknown value */
        INVALID
    };

    int primaryColorIndex = 0;

    Bitmap bitmap;

    Palette palette;

    QScopedPointer<FileBuffer> fileBuffer;

    QString supFileName;

    QVector<SubPictureBD*> subPictures;

    SubtitleProcessor* subtitleProcessor = 0;

    void decode(SubPictureBD &subPicture);
    void parsePCS(SupSegment* segment, SubPictureBD &subPicture, QString msg);
    void parseWDS(SupSegment* segment, SubPictureBD &subPicture);

    int getFpsId(double fps);
    int parsePDS(SupSegment* segment, SubPictureBD &subPicture, QString msg);

    double getFpsFromID(int id);

    bool picMergable(SubPictureBD *a, SubPictureBD *b);
    bool parseODS(SupSegment* segment, SubPictureBD &subPicture, QString msg);

    Bitmap decodeImage(SubPictureBD &subPicture, int transIdx);

    CompositionState getCompositionState(SupSegment* segment);

    Palette decodePalette(SubPictureBD &subPicture);

    QVector<uchar> encodeImage(Bitmap &bm);

    SupSegment* readSegment(int offset);

    QVector<uchar> packetHeader =
    {
        0x50, 0x47,                         // 0:  "PG"
        0x00, 0x00, 0x00, 0x00,             // 2:  PTS - presentation time stamp
        0x00, 0x00, 0x00, 0x00,             // 6:  DTS - decoding time stamp
        0x00,                               // 10: segment_type
        0x00, 0x00,                         // 11: segment_length (bytes following till next PG)
    };

    QVector<uchar> headerPCSStart =
    {
        0x00, 0x00, 0x00, 0x00,             // 0: video_width, video_height
        0x10,                               // 4: hi nibble: frame_rate (0x10=24p), lo nibble: reserved
        0x00, 0x00,                         // 5: composition_number (increased by start and end header)
        0x80,                               // 7: composition_state (0x80: epoch start)
        0x00,                               // 8: palette_update_flag (0x80), 7bit reserved
        0x00,                               // 9: palette_id_ref (0..7)
        0x01,                               // 10: number_of_composition_objects (0..2)
        0x00, 0x00,                         // 11: 16bit object_id_ref
        0x00,                               // 13: window_id_ref (0..1)
        0x00,                               // 14: object_cropped_flag: 0x80, forced_on_flag = 0x040, 6bit reserved
        0x00, 0x00, 0x00, 0x00              // 15: composition_object_horizontal_position, composition_object_vertical_position
    };

    QVector<uchar> headerPCSEnd =
    {
        0x00, 0x00, 0x00, 0x00,             // 0: video_width, video_height
        0x10,                               // 4: hi nibble: frame_rate (0x10=24p), lo nibble: reserved
        0x00, 0x00,                         // 5: composition_number (increased by start and end header)
        0x00,                               // 7: composition_state (0x00: normal)
        0x00,                               // 8: palette_update_flag (0x80), 7bit reserved
        0x00,                               // 9: palette_id_ref (0..7)
        0x00,                               // 10: number_of_composition_objects (0..2)
    };


    QVector<uchar> headerODSFirst =
    {
        0x00, 0x00,                         // 0: object_id
        0x00,                               // 2: object_version_number
        0xC0,                               // 3: first_in_sequence (0x80), last_in_sequence (0x40), 6bits reserved
        0x00, 0x00, 0x00,                   // 4: object_data_length - full RLE buffer length (including 4 bytes size info)
        0x00, 0x00, 0x00, 0x00,             // 7: object_width, object_height
    };

    QVector<uchar> headerODSNext =
    {
        0x00, 0x00,                         // 0: object_id
        0x00,                               // 2: object_version_number
        0x40,                               // 3: first_in_sequence (0x80), last_in_sequence (0x40), 6bits reserved
    };

    QVector<uchar> headerWDS =
    {
        0x01,                               // 0 : number of windows (currently assumed 1, 0..2 is legal)
        0x00,                               // 1 : window id (0..1)
        0x00, 0x00, 0x00, 0x00,             // 2 : x-ofs, y-ofs
        0x00, 0x00, 0x00, 0x00              // 6 : width, height
    };
};

#endif // SUPBD_H
