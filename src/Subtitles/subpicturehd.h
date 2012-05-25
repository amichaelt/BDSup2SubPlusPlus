#ifndef SUBPICTUREHD_H
#define SUBPICTUREHD_H

#include "subpicture.h"

class SubPictureHD : public SubPicture
{
public:
    SubPictureHD();
    SubPictureHD(SubPictureHD& other);
    SubPictureHD(SubPictureHD* other);

    int paletteOfs = 0;
    int alphaOfs = 0;
    int imageBufferSize = 0;
    int imageBufferOfsEven = 0;
    int imageBufferOfsOdd = 0;
};

#endif // SUBPICTUREHD_H
