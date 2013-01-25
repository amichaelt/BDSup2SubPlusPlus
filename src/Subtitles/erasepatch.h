/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
 * and Copyright 2012 Miklos Juhasz (mjuhasz)
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

#ifndef ERASEPATCH_H
#define ERASEPATCH_H

class ErasePatch
{
public:
    ErasePatch();
    ErasePatch(const ErasePatch* other);
    ErasePatch(const ErasePatch& other);
    ErasePatch(int x, int y, int width, int height);

    int x() { return x1; }
    void setX(int x) { x1 = x; }
    int y() { return y1; }
    void setY(int y) { y1 = y; }
    int width() { return w; }
    void setWidth(int width) { w = width; }
    int height() { return h; }
    void setHeight(int height) { h = height; }

private:
    int x1;
    int y1;
    int w;
    int h;
};

#endif // ERASEPATCH_H
