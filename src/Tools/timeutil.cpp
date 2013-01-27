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

#include "timeutil.h"

#include <QStringList>
#include <QVector>

TimeUtil::TimeUtil()
{
}

qint64 TimeUtil::timeStrXmlToPTS(QString s, double fps)
{
    if (timePattern.exactMatch(s) && timePattern.indexIn(s) != -1)
    {
        QStringList m = timePattern.capturedTexts();
        qint64 hour = m[1].toInt();
        qint64 min = m[2].toInt();
        qint64 sec = m[3].toInt();
        qint64 frames = m[4].toInt();

        qint64 temp = hour * 60;
        temp += min;
        temp *= 60;
        temp += sec;
        temp *= 1000;
        return (temp +(int)(((frames / fps) * 1000.0) + 0.5)) * 90;
    }
    else
    {
        return -1;
    }
}

qint64 TimeUtil::timeStrToPTS(QString s, bool *ok)
{
    *ok = false;
    bool timestampIsNegative = s[0] == '-';
    QString temp = timestampIsNegative ? s.mid(1) : s;
    if (timePattern.exactMatch(temp) && timePattern.indexIn(temp) != -1)
    {
        QStringList m = timePattern.capturedTexts();
        qint64 hour = m[1].toInt();
        qint64 min = m[2].toInt();
        qint64 sec = m[3].toInt();
        qint64 ms  = m[4].toInt();

        qint64 temp = hour * 60;
        temp += min;
        temp *= 60;
        temp += sec;
        temp *= 1000;
        *ok = true;
        qint64 ret = (temp + ms) * 90;
        return timestampIsNegative ? -ret : ret;
    }
    else
    {
        return -1;
    }
}

QString TimeUtil::ptsToTimeStrXml(qint64 pts, double fps)
{
    QVector<int> time = msToTime((pts + 45) / 90);
    return QString("%1:%2:%3:%4").arg(QString::number(time[0]), 2, QChar('0'))
                                 .arg(QString::number(time[1]), 2, QChar('0'))
                                 .arg(QString::number(time[2]), 2, QChar('0'))
                                 .arg(QString::number((int)(((fps * time[3]) / 1000.0) + 0.5)), 2, QChar('0'));
}

QString TimeUtil::ptsToTimeStr(qint64 pts)
{
    bool ptsIsNegative = pts < 0;
    pts = ptsIsNegative ? -pts : pts;
    QVector<int> time = msToTime((pts + 45) / 90);
    return QString("%1%2:%3:%4.%5").arg(ptsIsNegative ? "-" : "")
                                   .arg(QString::number(time[0]), 2, QChar('0'))
                                   .arg(QString::number(time[1]), 2, QChar('0'))
                                   .arg(QString::number(time[2]), 2, QChar('0'))
                                   .arg(QString::number(time[3]), 3, QChar('0'));
}

QString TimeUtil::ptsToTimeStrIdx(qint64 pts)
{
    bool ptsIsNegative = pts < 0;
    pts = ptsIsNegative ? -pts : pts;
    QVector<int> time = msToTime((pts + 45) / 90);
    return QString("%1%2:%3:%4:%5").arg(ptsIsNegative ? "-" : "")
                                   .arg(QString::number(time[0]), 2, QChar('0'))
                                   .arg(QString::number(time[1]), 2, QChar('0'))
                                   .arg(QString::number(time[2]), 2, QChar('0'))
                                   .arg(QString::number(time[3]), 3, QChar('0'));
}

QVector<int> TimeUtil::msToTime(qint64 ms)
{
    QVector<int> time(4);
    // time[0] = hours
    time.replace(0, (int)(ms / (60 * 60 * 1000)));
    ms -= (time[0] * 60 * 60 * 1000);
    // time[1] = minutes
    time.replace(1, (int)(ms / (60 * 1000)));
    ms -= (time[1] * 60 * 1000);
    // time[2] = seconds
    time.replace(2, (int)(ms / 1000));
    ms -= (time[2] * 1000);
    time.replace(3, (int)ms);
    return time;
}
