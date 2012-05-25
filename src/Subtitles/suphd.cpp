#include "suphd.h"

SupHD::SupHD(QString fileName, SubtitleProcessor *subtitleProcessor) :
    supFileName(fileName)
{
    this->subtitleProcessor = subtitleProcessor;
}

Palette *SupHD::getPalette()
{
    //TODO: implement
    throw 10;
}

Bitmap *SupHD::getBitmap()
{
    //TODO: implement
    throw 10;
}

QImage *SupHD::getImage()
{
    //TODO: implement
    throw 10;
}

QImage *SupHD::getImage(Bitmap *bitmap)
{
    //TODO: implement
    throw 10;
}

int SupHD::getPrimaryColorIndex()
{
    //TODO: implement
    throw 10;
}

void SupHD::decode(int index)
{
    //TODO: implement
    throw 10;
}

int SupHD::getNumFrames()
{
    //TODO: implement
    throw 10;
}

int SupHD::getNumForcedFrames()
{
    //TODO: implement
    throw 10;
}

bool SupHD::isForced(int index)
{
    //TODO: implement
    throw 10;
}

void SupHD::close()
{
    //TODO: implement
    throw 10;
}

long SupHD::getEndTime(int index)
{
    //TODO: implement
    throw 10;
}

long SupHD::getStartTime(int index)
{
    //TODO: implement
    throw 10;
}

long SupHD::getStartOffset(int index)
{
    //TODO: implement
    throw 10;
}

SubPicture *SupHD::getSubPicture(int index)
{
    //TODO: implement
    throw 10;
}

void SupHD::readAllSupFrames()
{
    //TODO: implement
    throw 10;
}
