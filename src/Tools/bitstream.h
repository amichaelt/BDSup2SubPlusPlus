#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <QVector>

class BitStream
{
public:
    BitStream(QVector<uchar> buffer);

    int bitsLeft();
    int readBits(int n);
    void syncToByte();

private:
    QVector<uchar> buf = QVector<uchar>();
    int byteOfs;
    int b;
    int bits;
};

#endif // BITSTREAM_H
