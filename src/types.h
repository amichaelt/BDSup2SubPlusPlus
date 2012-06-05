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

#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QStringList>
#include <QVector>

const QString progName = "BDSup2Sub++";
const QString progNameVer = progName + " 1.0.0";
const QString authorDate = "0xdeadbeef, mjuhasz, Adam T.";

const QVector<uchar> defaultPalR = {
    0x00, 0xf0, 0xcc, 0x99,
    0x33, 0x11, 0xfa, 0xbb,
    0x33, 0x11, 0xfa, 0xbb,
    0xfa, 0xbb, 0x33, 0x11
};
const QVector<uchar> defaultPalG = {
    0x00, 0xf0, 0xcc, 0x99,
    0x33, 0x11, 0x33, 0x11,
    0xfa, 0xbb, 0xfa, 0xbb,
    0x33, 0x11, 0xfa, 0xbb
};
const QVector<uchar> defaultPalB = {
    0x00, 0xf0, 0xcc, 0x99,
    0xfa, 0xbb, 0x33, 0x11,
    0x33, 0x11, 0x33, 0x11,
    0xfa, 0xbb, 0xfa, 0xbb,
};
const QVector<uchar> defaultAlpha = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

/** Enumeration for set/clear/keep states */
enum class SetState {
    /** keep (no change) */
    KEEP,
    /** set */
    SET,
    /** clear */
    CLEAR
};

enum class Resolution {
    /** NTSC: 720x480 */
    NTSC,
    /** PAL: 720x576 */
    PAL,
    /** 720p: 1280x720 */
    HD_720,
    /** 1440x1080 */
    HD_1440x1080,
    /** 1080p: 1920x1080 */
    HD_1080
};

enum class InputMode {
    /** DVD SUB/IDX (VobSub) stream */
    VOBSUB,
    /** Blu-Ray SUP stream */
    BDSUP,
    /** Sony BDN XML (+PNGs) */
    XML,
    /** HD-DVD SUP stream */
    HDDVDSUP,
    /** DVD SUP/IFO stream */
    SUPIFO
};

/** Enumeration of output modes */
enum class OutputMode {
    /** DVD SUB/IDX (VobSub) stream */
    VOBSUB,
    /** DVD SUP/IFO stream */
    SUPIFO,
    /** Blu-Ray SUP stream */
    BDSUP,
    /** Sony BDN XML (+PNGs) */
    XML
};

/** Enumeration of modes for moving of captions in Y direction */
enum class MoveModeY {
    /** keep position - don't move */
    KEEP,
    /** move inside bounds */
    INSIDE,
    /** move outside bounds */
    OUTSIDE
};

/** Enumeration of modes for moving of captions in X direction */
enum class MoveModeX {
    /** keep position - don't move */
    KEEP,
    /** move to center */
    CENTER,
    /** move to left border */
    LEFT,
    /** move to right border */
    RIGHT,
};

enum class ScalingFilters {
    /** Bilinear filter */
    BILINEAR,
    /** Triangle filter */
    TRIANGLE,
    /** Bicubic filter */
    BICUBIC,
    /** Bell filter */
    BELL,
    /** Bicubic spline filter */
    BSPLINE,
    /** Hermite filter */
    HERMITE,
    /** Lanczos3 filter */
    LANCZOS3,
    /** Mitchell filter */
    MITCHELL,
};

enum class PaletteMode {
    /** keep existing palette */
    KEEP_EXISTING,
    /** create new */
    CREATE_NEW,
    /** create new dithered */
    CREATE_DITHERED
};

enum class StreamID
{
    /** Blu-Ray SUP stream */
    BDSUP,
    /** HD-DVD SUP or DVD SUP stream (same ID) */
    SUP,
    /** DVD VobSub SUB stream*/
    DVDSUB,
    /** Sony BDN XML */
    XML,
    /** DVD IFO */
    IFO,
    /** DVD VobSub IDX */
    IDX,
    /** UNKNOWN */
    UNKNOWN
};

enum class RunType {
    /** read a SUP stream */
    READSUP,
    /** read a SUP stream */
    READXML,
    /** read a VobSub stream */
    READVOBSUB,
    /** read a SUP/IFO stream */
    READSUPIFO,
    /** write a VobSub stream */
    CREATESUB,
    /** write a BD-SUP stream */
    CREATESUP,
    /** move all captions */
    MOVEALL
};

/** Frames per seconds for 24p (23.976) */
constexpr double FPS_24P  = 24000.0/1001;
/** Frames per seconds for wrong 24P (23.975) */
constexpr double FPS_23_975 = 23.975;
/** Frames per seconds for 24Hz (24.0) */
constexpr double FPS_24HZ = 24.0;
/** Frames per seconds for PAL progressive (25.0) */
constexpr double FPS_PAL  = 25.0;
/** Frames per seconds for NTSC progressive (29.97) */
constexpr double FPS_NTSC = 30000.0/1001;
/** Frames per seconds for PAL interlaced (50.0) */
constexpr double FPS_PAL_I  = 50.0;
/** Frames per seconds for NTSC interlaced (59.94) */
constexpr double FPS_NTSC_I = 60000.0/1001;

#endif // TYPES_H
