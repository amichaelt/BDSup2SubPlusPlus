#include "supdvb.h"
#include "subtitleprocessor.h"
#include "Tools/filebuffer.h"

SupDVB::SupDVB(QString supFileName, SubtitleProcessor* subtitleProcessor) :
    supFileName(supFileName)
{
    this->subtitleProcessor = subtitleProcessor;
}

SupDVB::~SupDVB()
{
    if (!fileBuffer.isNull())
    {
        fileBuffer.reset();
    }
}

void SupDVB::readAllSupFrames()
{
    fileBuffer.reset(new FileBuffer(supFileName));

    long ofs = 0;
    long size = fileBuffer->getSize();

    if (size <= 0)
    {
        throw QString("SUP file is empty. No subtitles can be read.");
    }

    emit maxProgressChanged(size);

    int i = 0;
    do
    {
        subtitleProcessor->printX(QString("# %1\n").arg(++i));

        emit currentProgressChanged(i);

        subtitleProcessor->print(QString("Ofs: %1\n").arg(QString::number(ofs, 16), 8, QChar('0')));

        ofs = readSupFrame(ofs);
    } while (ofs < size);
    emit currentProgressChanged(i);
}

long SupDVB::readSupFrame(long ofs)
{
    long startOfs = ofs;
    if (fileBuffer->getWord(ofs) != 0x2000)
    {
        throw QString("Missing packet identifier at ofs: %1").arg(QString::number(ofs, 16), 8, QChar('0'));
    }

    ofs += 2;

    while (fileBuffer->getByte(ofs) == SYNC_BYTE)
    {
        ++ofs;
        quint8 segmentType = fileBuffer->getByte(ofs);
        ++ofs;
        quint16 pageId = fileBuffer->getWord(ofs);
        ofs += 2;
        quint16 segmentLength = fileBuffer->getWord(ofs);
        ofs += 2;
        quint16 segmentDataField = fileBuffer->getWord(ofs);
    }

}
