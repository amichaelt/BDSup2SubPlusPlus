#-------------------------------------------------
#
# Project created by QtCreator 2011-12-26T16:10:17
#
#-------------------------------------------------

QT       += core gui xml
QMAKE_CXXFLAGS += -std=c++0x
TARGET = bdsup2sub++
TEMPLATE = app


SOURCES += main.cpp\
        bdsup2sub.cpp \
    zoomableimagearea.cpp \
    editpane.cpp \
    Filters/trianglefilter.cpp \
    Filters/mitchellfilter.cpp \
    Filters/lanczos3filter.cpp \
    Filters/hermitefilter.cpp \
    Filters/bsplinefilter.cpp \
    Filters/bicubicfilter.cpp \
    Filters/bellfilter.cpp \
    Filters/filterop.cpp \
    Subtitles/subdvd.cpp \
    Subtitles/palette.cpp \
    Subtitles/bitmap.cpp \
    Subtitles/subtitleprocessor.cpp \
    Subtitles/subpicture.cpp \
    Subtitles/erasepatch.cpp \
    Subtitles/subpicturedvd.cpp \
    Subtitles/imageobjectfragment.cpp \
    progressdialog.cpp \
    Subtitles/supdvd.cpp \
    Tools/filebuffer.cpp \
    conversiondialog.cpp \
    Filters/filters.cpp \
    Subtitles/palettebitmap.cpp \
    Tools/timeutil.cpp \
    Subtitles/suphd.cpp \
    Subtitles/supbd.cpp \
    Subtitles/subpicturebd.cpp \
    Subtitles/imageobject.cpp \
    Subtitles/paletteinfo.cpp \
    Subtitles/substreamdvd.cpp \
    Subtitles/subpicturehd.cpp \
    Tools/bitstream.cpp \
    Subtitles/supxml.cpp \
    Subtitles/subpicturexml.cpp \
    Tools/quantizefilter.cpp \
    exportdialog.cpp \
    Tools/numberutil.cpp

HEADERS  += bdsup2sub.h \
    zoomableimagearea.h \
    editpane.h \
    types.h \
    Filters/trianglefilter.h \
    Filters/mitchellfilter.h \
    Filters/lanczos3filter.h \
    Filters/hermitefilter.h \
    Filters/filter.h \
    Filters/bsplinefilter.h \
    Filters/bicubicfilter.h \
    Filters/bellfilter.h \
    Filters/filterop.h \
    Subtitles/substream.h \
    Subtitles/substreamdvd.h \
    Subtitles/subdvd.h \
    Subtitles/palette.h \
    Subtitles/bitmap.h \
    Subtitles/subtitleprocessor.h \
    Subtitles/subpicture.h \
    Subtitles/erasepatch.h \
    Subtitles/subpicturedvd.h \
    Subtitles/imageobjectfragment.h \
    progressdialog.h \
    Subtitles/supdvd.h \
    Tools/filebuffer.h \
    conversiondialog.h \
    Filters/filters.h \
    Subtitles/palettebitmap.h \
    Tools/timeutil.h \
    Subtitles/suphd.h \
    Subtitles/supbd.h \
    Subtitles/subpicturebd.h \
    Subtitles/imageobject.h \
    Subtitles/paletteinfo.h \
    Subtitles/subpicturehd.h \
    Tools/bitstream.h \
    Subtitles/supxml.h \
    Subtitles/subpicturexml.h \
    Tools/quantizefilter.h \
    exportdialog.h \
    Tools/numberutil.h

FORMS    += bdsup2sub.ui \
    progressdialog.ui \
    conversiondialog.ui \
    exportdialog.ui

RESOURCES += \
    bdsup2sub.qrc
