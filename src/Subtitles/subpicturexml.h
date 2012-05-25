#ifndef SUBPICTUREXML_H
#define SUBPICTUREXML_H

#include "subpicture.h"

#include <QString>

class SubPictureXML : public SubPicture
{
public:
    SubPictureXML();
    SubPictureXML(SubPictureXML& other);
    SubPictureXML(SubPictureXML* other);

    int originalX;
    int originalY;
    QString fileName;

    void setOriginal() { originalX = getOfsX(); originalY = getOfsY(); }
};

#endif // SUBPICTUREXML_H
