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

#include "quantizefilter.h"
#include <algorithm>
#include <QHash>
#include <QList>

void QuantizeFilter::setNumColors(int numColors)
{
    this->numColors = std::min(std::max(numColors, 8), 256);
}

QVector<QRgb> QuantizeFilter::quantize(QImage *inImage, QImage *outImage, int width, int height,
                                      int numColors, bool dither, bool serpentine)
{
    QuantizeFilter::OctTreeQuantizer quantizer;
    quantizer.setup(numColors);
    quantizer.addPixels(inImage);
    QVector<QRgb> table = quantizer.buildColorTable();

    if (dither)
    {
        for (int y = 0; y < height; ++y)
        {
            bool reverse = serpentine && (y & 1) == 1;
            int direction;
            int index = 0;
            if (reverse)
            {
                index = width;
                direction = -1;
            }
            else
            {
                direction = 1;
            }
            QRgb* inPixels = (QRgb*)inImage->scanLine(y);
            uchar* outPixels = outImage->scanLine(y);
            for (int x = 0; x < width; ++x)
            {
                QRgb rgb1 = inPixels[index];
                int idx = quantizer.getIndexForColor(rgb1);
                QRgb rgb2 = table[idx];

                outPixels[index] = (uchar)(idx & 0xff);

                int a1 = qAlpha(rgb1);
                int r1 = qRed(rgb1);
                int g1 = qGreen(rgb1);
                int b1 = qBlue(rgb1);

                int a2 = qAlpha(rgb2);
                int r2 = qRed(rgb2);
                int g2 = qGreen(rgb2);
                int b2 = qBlue(rgb2);

                int ea = a1 - a2;
                int er = r1 - r2;
                int eg = g1 - g2;
                int eb = b1 - b2;

                for (int i = -1; i <= 1; ++i)
                {
                    int iy = i + y;
                    if (0 <= iy && iy < height)
                    {
                        for (int j = -1; j <= 1; ++j)
                        {
                            int jx = j + x;
                            if (0 <= jx && jx < width)
                            {
                                int w;
                                if (reverse)
                                {
                                    w = matrix[(((i + 1) * 3) - j) + 1];
                                }
                                else
                                {
                                    w = matrix[(((i + 1) * 3) + j) + 1];
                                }
                                if (w != 0)
                                {
                                    int k = reverse ? (index - j) : (index + j);
                                    rgb1 = inPixels[k];

                                    a1 = qAlpha(rgb1);
                                    r1 = qRed(rgb1);
                                    g1 = qGreen(rgb1);
                                    b1 = qBlue(rgb1);
                                    a1 += ((ea * w) / sum);
                                    r1 += ((er * w) / sum);
                                    g1 += ((eg * w) / sum);
                                    b1 += ((eb * w) / sum);
                                    inPixels[k] = qRgba(clamp(r1), clamp(g1), clamp(b1), clamp(a1));
                                }
                            }
                        }
                    }
                }
                index += direction;
            }
        }
    }

    // create palette
    QHash<QRgb, int> p;

    for (int y = 0; y < inImage->height(); ++y)
    {
        QRgb* inPixels = (QRgb*)inImage->scanLine(y);
        uchar* outPixels = outImage->scanLine(y);
        for (int x = 0; x < inImage->width(); ++x)
        {
            int color;
            if (dither)
            {
                color = table[outPixels[x] & 0xff];
            }
            else
            {
                color = table[quantizer.getIndexForColor(inPixels[x])];
            }
            int idx = p.size();
            if (p.contains(color))
            {
                int idxEx = p.value(color);
                idx = idxEx;
            }
            else
            {
                p.insert(color, idx);
            }
            outPixels[x] = (uchar)(idx);
        }
    }

    return p.keys().toVector();
}

int QuantizeFilter::clamp(int c)
{
    if (c < 0)
    {
        return 0;
    }
    if (c > 255)
    {
        return 255;
    }
    return c;
}

void QuantizeFilter::OctTreeQuantizer::setup(int numColors)
{
    maximumColors = numColors;
    reduceColors = std::max(512, numColors * 2);
}

void QuantizeFilter::OctTreeQuantizer::addPixels(QImage* image)
{
    for (int y = 0; y < image->height(); ++y)
    {
        QRgb* pixels = (QRgb*)image->scanLine(y);
        for (int x = 0; x < image->width(); ++x)
        {
            insertColor(pixels[x]);
            if (colors > reduceColors)
            {
                reduceTree(reduceColors);
            }
        }
    }
}

int QuantizeFilter::OctTreeQuantizer::getIndexForColor(QRgb argb)
{
    int alpha = qAlpha(argb);
    int red = qRed(argb);
    int green = qGreen(argb);
    int blue = qBlue(argb);

    OctTreeNode* node = root;

    for (int level = 0; level <= MAX_LEVEL; ++level)
    {
        OctTreeNode* child;
        int bit = 0x80 >> level;

        int index = 0;
        if ((alpha & bit) != 0)
        {
            index += 8;
        }
        if ((red & bit) != 0)
        {
            index += 4;
        }
        if ((green & bit) != 0)
        {
            index += 2;
        }
        if ((blue & bit) != 0)
        {
            index += 1;
        }

        child = node->leaf[index];

        if (child == 0)
        {
            return node->index;
        }
        else if (child->isLeaf)
        {
            return child->index;
        }
        else
        {
            node = child;
        }
    }
    return 0;
}

QVector<QRgb> QuantizeFilter::OctTreeQuantizer::buildColorTable()
{
    QVector<QRgb> table(colors);
    buildColorTable(root, table, 0);
    return table;
}

void QuantizeFilter::OctTreeQuantizer::buildColorTable(QVector<QRgb> inPixels, QVector<QRgb>& table)
{
    int count = inPixels.size();
    maximumColors = table.size();
    for (int i = 0; i < count; ++i)
    {
        insertColor(inPixels[i]);
        if (colors > reduceColors)
        {
            reduceTree(reduceColors);
        }
    }
    if (colors > maximumColors)
    {
        reduceTree(maximumColors);
    }
    buildColorTable(root, table, 0);
}

void QuantizeFilter::OctTreeQuantizer::insertColor(QRgb rgb)
{
    int alpha = qAlpha(rgb);
    int red = qRed(rgb);
    int green = qGreen(rgb);
    int blue = qBlue(rgb);

    OctTreeNode* node = root;

    for (int level = 0; level <= MAX_LEVEL; ++level)
    {
        OctTreeNode* child;
        int bit = 0x80 >> level;

        int index = 0;
        if ((alpha & bit) != 0)
        {
            index += 8;
        }
        if ((red & bit) != 0)
        {
            index += 4;
        }
        if ((green & bit) != 0)
        {
            index += 2;
        }
        if ((blue & bit) != 0)
        {
            index += 1;
        }

        child = node->leaf[index];

        if (child == 0)
        {
            node->children++;

            child = new OctTreeNode;
            child->parent = node;
            node->leaf.replace(index, child);
            node->isLeaf = false;
            nodes++;
            colorList[level].push_back(child);

            if (level == MAX_LEVEL)
            {
                child->isLeaf = true;
                child->count = 1;
                child->totalAlpha = alpha;
                child->totalRed = red;
                child->totalGreen = green;
                child->totalBlue = blue;
                child->level = level;
                colors++;
                return;
            }

            node = child;
        }
        else if (child->isLeaf)
        {
            child->count++;
            child->totalAlpha += alpha;
            child->totalRed += red;
            child->totalGreen += green;
            child->totalBlue += blue;
            return;
        }
        else
        {
            node = child;
        }
    }
}

void QuantizeFilter::OctTreeQuantizer::reduceTree(int numColors)
{
    for (int level = MAX_LEVEL - 1; level >= 0; --level)
    {
        QVector<OctTreeNode*> v = colorList[level];
        if (!v.isEmpty() && v.size() > 0)
        {
            for (int j = 0; j < v.size(); j++)
            {
                OctTreeNode* node = v.at(j);
                if (node->children > 0)
                {
                    for (int i = 0; i < node->leaf.size(); i++)
                    {
                        OctTreeNode* child = node->leaf[i];
                        if (child != 0)
                        {
                            node->count += child->count;
                            node->totalAlpha += child->totalAlpha;
                            node->totalRed += child->totalRed;
                            node->totalGreen += child->totalGreen;
                            node->totalBlue += child->totalBlue;
                            node->leaf[i] = 0;
                            node->children--;
                            --colors;
                            --nodes;
                            colorList[level + 1].remove(colorList[level + 1].indexOf(child));
                        }
                    }
                    node->isLeaf = true;
                    ++colors;
                    if (colors <= numColors)
                    {
                        return;
                    }
                }
            }
        }
    }
}

int QuantizeFilter::OctTreeQuantizer::buildColorTable(QuantizeFilter::OctTreeQuantizer::OctTreeNode *node, QVector<QRgb>& table, int index)
{
    if (colors > maximumColors)
    {
        reduceTree(maximumColors);
    }

    if (node->isLeaf)
    {
        int count = node->count;
        table.replace(index, qRgba(node->totalRed / count,
                                   node->totalGreen / count,
                                   node->totalBlue / count,
                                   node->totalAlpha / count));
        node->index = index++;
    }
    else
    {
        for (int i = 0; i < node->leaf.size(); ++i)
        {
            if (node->leaf[i] != 0)
            {
                node->index = index;
                index = buildColorTable(node->leaf[i], table, index);
            }
        }
    }
    return index;
}
