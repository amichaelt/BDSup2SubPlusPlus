#include "numberutil.h"

void NumberUtil::setString(QVector<uchar> &buf, int index, QString string)
{
    for (int i = 0; i < string.size(); ++i)
    {
        buf.replace(index + i, (uchar)string.at(i).toAscii());
    }
}
