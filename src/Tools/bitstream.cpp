#include "bitstream.h"

BitStream::BitStream(QVector<uchar> buffer) :
    buf(buffer),
    byteOfs(0),
    b(buf[0] & 0xff),
    bits(8)
{
}

int BitStream::bitsLeft()
{
    return (8 * (buf.size() - byteOfs)) - (8 - bits);
}

int BitStream::readBits(int n)
{
    int retval = 0;
    while (n > 0)
    {
        // bit by bit
        retval <<= 1;
        if ((b & 0x80) == 0x80)
        {
            retval |= 1; // get next bit
        }
        b <<= 1;
        n--;
        if (--bits == 0)
        {
            if (byteOfs < (buf.size() - 1))
            {
                b = buf[++byteOfs] & 0xff;
                bits = 8;
            }
            else
            {
                bits = 0;
            }
        }
    }
    return retval;
}

void BitStream::syncToByte()
{
    if (bits !=8)
    {
        if (byteOfs < (buf.size() - 1))
        {
            b = buf[++byteOfs] & 0xff;
            bits = 8;
        }
        else
        {
            bits = 0;
        }
    }
}
