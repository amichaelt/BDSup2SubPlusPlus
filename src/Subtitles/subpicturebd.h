#ifndef SUBPICTUREBD_H
#define SUBPICTUREBD_H

#include "subpicture.h"
#include "imageobjectfragment.h"
#include "paletteinfo.h"
#include <QVector>

class ImageObject;
class PaletteInfo;

class SubPictureBD : public SubPicture
{
public:
    SubPictureBD();
    SubPictureBD(SubPictureBD* other);

    int objectID = 0;
    QVector<ImageObject*> imageObjectList = QVector<ImageObject*>();
    int winWidth = 0;
    int winHeight = 0;
    int xWinOfs = 0;
    int yWinOfs = 0;
    int type = 0;
    QVector<QVector<PaletteInfo*>*> palettes = QVector<QVector<PaletteInfo*>*>();

    ImageObject* getImgObj(int index) { return imageObjectList.at(index); }
    ImageObject* getImgObj() { return imageObjectList.at(objectID); }
    int getImageWidth();
    int getImageHeight();
    int getOfsX();
    int getOfsY();
};

#endif // SUBSPICTUREBD_H
