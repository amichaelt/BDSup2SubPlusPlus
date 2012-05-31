#include "numberutil.h"

void NumberUtil::setByte(QVector<uchar> &buf, int index, int value)
{
    buf.replace(index, (uchar)value);
}

void NumberUtil::setWord(QVector<uchar> &buf, int index, int value)
{
    buf.replace(index, (uchar)(value >> 8));
    buf.replace(index + 1, (uchar)value);
}

void NumberUtil::setDWord(QVector<uchar> &buf, int index, int value)
{
    buf.replace(index, (uchar)(value >> 24));
    buf.replace(index + 1, (uchar)(value >> 16));
    buf.replace(index + 2, (uchar)(value >> 8));
    buf.replace(index + 3, (uchar)value);
}

void NumberUtil::setString(QVector<uchar> &buf, int index, QString string)
{
    for (int i = 0; i < string.size(); ++i)
    {
        buf.replace(index + i, (uchar)string.at(i).toAscii());
    }
}
