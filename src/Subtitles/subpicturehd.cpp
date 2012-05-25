#include "subpicturehd.h"

SubPictureHD::SubPictureHD()
{
}

SubPictureHD::SubPictureHD(SubPictureHD &other) :
    paletteOfs(other.paletteOfs),
    alphaOfs(other.alphaOfs),
    imageBufferSize(other.imageBufferSize),
    imageBufferOfsEven(other.imageBufferOfsEven),
    imageBufferOfsOdd(other.imageBufferOfsOdd)
{
}

SubPictureHD::SubPictureHD(SubPictureHD *other) :
    paletteOfs(other->paletteOfs),
    alphaOfs(other->alphaOfs),
    imageBufferSize(other->imageBufferSize),
    imageBufferOfsEven(other->imageBufferOfsEven),
    imageBufferOfsOdd(other->imageBufferOfsOdd)
{
}
