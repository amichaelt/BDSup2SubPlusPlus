/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
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
