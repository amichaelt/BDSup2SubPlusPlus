#include "subpicturexml.h"

SubPictureXML::SubPictureXML()
{
}

SubPictureXML::SubPictureXML(SubPictureXML &other) :
    originalX(other.originalX),
    originalY(other.originalY),
    fileName(other.fileName)
{
}

SubPictureXML::SubPictureXML(SubPictureXML *other) :
    originalX(other->originalX),
    originalY(other->originalY),
    fileName(other->fileName)
{
}
