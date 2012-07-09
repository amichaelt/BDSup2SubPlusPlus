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

#ifndef QUANTIZEFILTER_H
#define QUANTIZEFILTER_H

#include <QColor>
#include <QImage>
#include <QVector>

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
        void addPixels(QImage &image);
        void buildColorTable(QVector<QRgb> inPixels, QVector<QRgb> &table);
        void setup(int numColors);

        int getIndexForColor(QRgb argb);

        QVector<QRgb> buildColorTable();

    private:
        static constexpr int MAX_LEVEL = 5;

        int colors = 0;
        int maximumColors = 256;
        int nodes = 0;
        int reduceColors = 512;

        OctTreeNode* root = new OctTreeNode;

        QVector<QVector<OctTreeNode*> > colorList = QVector<QVector<OctTreeNode*> >(MAX_LEVEL + 1);

        void insertColor(QRgb rgb);
        void reduceTree(int numColors);
        int buildColorTable(OctTreeNode* node, QVector<QRgb>& table, int index);
    };

public:
     void setNumColors(int numColors);
     void setDither(bool dither) { this->dither = dither; }
     void setSerpentine(bool serpentine) { this->serpentine = serpentine; }

     int clamp(int c);
     int getNumColors() { return numColors; }

     bool getDither() { return dither; }
     bool getSerpentine() { return serpentine; }

     QVector<QRgb> quantize(QImage inImage, QImage *outImage, int width, int height,
                           int numColors, bool dither, bool serpentine);

private:
    static constexpr int sum = 3 + 5 + 7 + 1;
    int numColors = 255;

    bool dither = false;
    bool serpentine = false;
};

#endif // QUANTIZEFILTER_H
