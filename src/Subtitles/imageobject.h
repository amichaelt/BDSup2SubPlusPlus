#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <QVector>

class ImageObjectFragment;

class ImageObject
{
public:
    ImageObject();

    QVector<ImageObjectFragment*> fragmentList = QVector<ImageObjectFragment*>();
    int paletteID = 0;
    int bufferSize = 0;
    int width = 0;
    int height = 0;
    int xOfs = 0;
    int yOfs = 0;
};

#endif // IMAGEOBJECT_H
