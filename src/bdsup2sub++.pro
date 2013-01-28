#-------------------------------------------------
#
# Project created by QtCreator 2011-12-26T16:10:17
#
#-------------------------------------------------

#!contains(QT_CONFIG,c++11) : error(bdsup2sub++ requires Qt to be configured with C++11 support)

#check Qt version
QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)

QT       += core xml
lessThan(QT_VER_MAJ, 4) {
QT       += gui
}
greaterThan(QT_VER_MAJ, 4) {
QT       -= gui
QT	 += widgets
}
CONFIG   += qt console qxt
#contains(QT_CONFIG,c++11): CONFIG += c++11
QXT      += core
QMAKE_CXXFLAGS += -std=c++11
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
    Subtitles/paletteinfo.cpp \
    Subtitles/substreamdvd.cpp \
    Subtitles/subpicturehd.cpp \
    Tools/bitstream.cpp \
    Subtitles/supxml.cpp \
    Subtitles/subpicturexml.cpp \
    Tools/quantizefilter.cpp \
    exportdialog.cpp \
    Tools/numberutil.cpp \
    editdialog.cpp \
    helpdialog.cpp \
    colordialog.cpp \
    framepalettedialog.cpp \
    movedialog.cpp \
    Subtitles/imageobject.cpp

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
    Tools/numberutil.h \
    editdialog.h \
    helpdialog.h \
    colordialog.h \
    framepalettedialog.h \
    movedialog.h

FORMS    += bdsup2sub.ui \
    progressdialog.ui \
    conversiondialog.ui \
    exportdialog.ui \
    editdialog.ui \
    helpdialog.ui \
    colordialog.ui \
    framepalettedialog.ui \
    movedialog.ui

RESOURCES += \
    bdsup2sub.qrc

RC_FILE = bdsup2sub.rc
