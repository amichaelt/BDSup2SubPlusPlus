#ifndef NUMBERUTIL_H
#define NUMBERUTIL_H

#include <QVector>
#include <QString>

class NumberUtil
{
public:
    static void setByte(QVector<uchar>& buf, int index, int value)
    {
        buf.replace(index, (uchar)value);
    }
    static void setWord(QVector<uchar>& buf, int index, int value)
    {
        buf.replace(index, (uchar)(value >> 8));
        buf.replace(index + 1, (uchar)value);
    }
    static void setDWord(QVector<uchar>& buf, int index, int value)
    {
        buf.replace(index, (uchar)(value >> 24));
        buf.replace(index + 1, (uchar)(value >> 16));
        buf.replace(index + 2, (uchar)(value >> 8));
        buf.replace(index + 3, (uchar)value);
    }
    static void setString(QVector<uchar> &buf, int index, QString string);
};

#endif // NUMBERUTIL_H
