#ifndef NUMBERUTIL_H
#define NUMBERUTIL_H

#include <QVector>
#include <QString>

class NumberUtil
{
public:
    static void setByte(QVector<uchar>& buf, int index, int value);
    static void setWord(QVector<uchar>& buf, int index, int value);
    static void setDWord(QVector<uchar>& buf, int index, int value);
    static void setString(QVector<uchar> &buf, int index, QString string);
};

#endif // NUMBERUTIL_H
