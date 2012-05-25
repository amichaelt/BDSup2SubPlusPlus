#include "subpicturebd.h"
#include "imageobject.h"

SubPictureBD::SubPictureBD()
{
}

SubPictureBD::SubPictureBD(SubPictureBD *other) :
    objectID(other->objectID),
    imageObjectList(other->imageObjectList),
    winWidth(other->winWidth),
    winHeight(other->winHeight),
    xWinOfs(other->xWinOfs),
    yWinOfs(other->yWinOfs),
    type(other->type),
    palettes(other->palettes)
{
}

int SubPictureBD::getImageWidth()
{
    return imageObjectList.at(objectID)->width;
}

int SubPictureBD::getImageHeight()
{
    return imageObjectList.at(objectID)->height;
}

int SubPictureBD::getOfsX()
{
    return imageObjectList.at(objectID)->xOfs;
}

int SubPictureBD::getOfsY()
{
    return imageObjectList.at(objectID)->yOfs;
}
