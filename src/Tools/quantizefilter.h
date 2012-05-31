/*
BDSup2Sub++ (C) 2012 Adam T.
Base on code Copyright 2006 Jerry Huxtable and modified for BDSup2Sub by Volker Oth/0xdeadbeef (extended to alpha component)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef QUANTIZEFILTER_H
#define QUANTIZEFILTER_H

#include <QVector>
#include <QColor>
#include <QImage>

/**
 * Floyd-Steinberg dithering matrix.
 */
static QVector<int> matrix = {
    0, 0, 0,
    0, 0, 7,
    3, 5, 1,
};

class QuantizeFilter
{
    class OctTreeQuantizer
    {
        struct OctTreeNode
        {
            int children = 0;
            int level = 0;
            OctTreeNode* parent = 0;
            QVector<OctTreeNode*> leaf = QVector<OctTreeNode*>(16);
            bool isLeaf = false;
            int count = 0;
            int totalAlpha = 0;
            int totalRed = 0;
            int totalGreen = 0;
            int totalBlue = 0;
            int index = 0;
        };

    public:
        void setup(int numColors);
        void addPixels(QImage* image);
        int getIndexForColor(QRgb argb);
        QVector<QRgb> buildColorTable();
        void buildColorTable(QVector<QRgb> inPixels, QVector<QRgb>& table);

    private:
        static constexpr int MAX_LEVEL = 5;
        int nodes = 0;
        OctTreeNode* root = new OctTreeNode;
        int maximumColors = 256;
        int reduceColors = 512;
        int colors = 0;
        QVector<QVector<OctTreeNode*> > colorList = QVector<QVector<OctTreeNode*> >(MAX_LEVEL + 1);

        void insertColor(QRgb rgb);
        void reduceTree(int numColors);
        int buildColorTable(OctTreeNode* node, QVector<QRgb>& table, int index);

    };

public:
    void setNumColors(int numColors);
    int getNumColors() { return numColors; }
    void setDither(bool dither) { this->dither = dither; }
    bool getDither() { return dither; }
    void setSerpentine(bool serpentine) { this->serpentine = serpentine; }
    bool getSerpentine() { return serpentine; }
    QVector<QRgb> quantize(QImage *inImage, QImage *outImage, int width, int height,
                           int numColors, bool dither, bool serpentine);
    int clamp(int c);

private:
    static constexpr int sum = 3 + 5 + 7 + 1;
    bool dither = false;
    bool serpentine = false;
    int numColors = 255;
};

#endif // QUANTIZEFILTER_H
