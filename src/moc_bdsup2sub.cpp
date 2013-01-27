/****************************************************************************
** Meta object code from reading C++ file 'bdsup2sub.h'
**
** Created: Sat Jan 26 17:55:59 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bdsup2sub.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bdsup2sub.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BDSup2Sub[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      35,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,   37,   46,   46, 0x0a,
      47,   86,   46,   46, 0x0a,
      98,   86,   46,   46, 0x0a,
     137,   86,   46,   46, 0x0a,
     164,   46,   46,   46, 0x0a,
     177,   46,   46,   46, 0x08,
     184,   46,   46,   46, 0x08,
     199,   46,   46,   46, 0x08,
     213,   46,   46,   46, 0x08,
     224,   46,   46,   46, 0x08,
     235,   46,   46,   46, 0x08,
     247,   46,   46,   46, 0x08,
     270,  293,   46,   46, 0x08,
     302,  317,   46,   46, 0x08,
     325,   46,   46,   46, 0x08,
     347,  379,   46,   46, 0x08,
     385,   46,   46,   46, 0x08,
     400,   46,   46,   46, 0x08,
     417,  440,   46,   46, 0x08,
     448,  440,   46,   46, 0x08,
     481,  440,   46,   46, 0x08,
     510,   46,   46,   46, 0x08,
     544,   46,   46,   46, 0x08,
     579,   46,   46,   46, 0x08,
     611,   46,   46,   46, 0x08,
     639,   46,   46,   46, 0x08,
     667,  720,   46,   46, 0x08,
     726,  720,   46,   46, 0x08,
     777,   46,   46,   46, 0x08,
     802,  720,   46,   46, 0x08,
     846,  720,   46,   46, 0x08,
     889,  720,   46,   46, 0x08,
     940,  720,   46,   46, 0x08,
     992,  720,   46,   46, 0x08,
    1043,  720,   46,   46, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BDSup2Sub[] = {
    "BDSup2Sub\0changeWindowTitle(QString)\0"
    "newTitle\0\0onLoadingSubtitleFileFinished(QString)\0"
    "errorString\0onWritingSubtitleFileFinished(QString)\0"
    "onMoveAllFinished(QString)\0convertSup()\0"
    "init()\0loadSettings()\0showAboutQt()\0"
    "openFile()\0saveFile()\0closeFile()\0"
    "onOperationCancelled()\0onAddLanguage(QString)\0"
    "language\0print(QString)\0message\0"
    "onRecentItemClicked()\0"
    "onEditPaneClicked(QMouseEvent*)\0event\0"
    "loadEditPane()\0loadHelpDialog()\0"
    "swapCrCb_toggled(bool)\0checked\0"
    "fixInvisibleFrames_toggled(bool)\0"
    "verbatimOutput_toggled(bool)\0"
    "editDefaultDVDPalette_triggered()\0"
    "editImportedDVDPalette_triggered()\0"
    "editDVDFramePalette_triggered()\0"
    "moveAllCaptions_triggered()\0"
    "resetCropOffset_triggered()\0"
    "on_subtitleLanguageComboBox_currentIndexChanged(int)\0"
    "index\0on_subtitleNumberComboBox_currentIndexChanged(int)\0"
    "openConversionSettings()\0"
    "on_paletteComboBox_currentIndexChanged(int)\0"
    "on_filterComboBox_currentIndexChanged(int)\0"
    "on_hiMedThresholdComboBox_currentIndexChanged(int)\0"
    "on_medLowThresholdComboBox_currentIndexChanged(int)\0"
    "on_alphaThresholdComboBox_currentIndexChanged(int)\0"
    "on_outputFormatComboBox_currentIndexChanged(int)\0"
};

void BDSup2Sub::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BDSup2Sub *_t = static_cast<BDSup2Sub *>(_o);
        switch (_id) {
        case 0: _t->changeWindowTitle((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->onLoadingSubtitleFileFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->onWritingSubtitleFileFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onMoveAllFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->convertSup(); break;
        case 5: _t->init(); break;
        case 6: _t->loadSettings(); break;
        case 7: _t->showAboutQt(); break;
        case 8: _t->openFile(); break;
        case 9: _t->saveFile(); break;
        case 10: _t->closeFile(); break;
        case 11: _t->onOperationCancelled(); break;
        case 12: _t->onAddLanguage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->print((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->onRecentItemClicked(); break;
        case 15: _t->onEditPaneClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 16: _t->loadEditPane(); break;
        case 17: _t->loadHelpDialog(); break;
        case 18: _t->swapCrCb_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->fixInvisibleFrames_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->verbatimOutput_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->editDefaultDVDPalette_triggered(); break;
        case 22: _t->editImportedDVDPalette_triggered(); break;
        case 23: _t->editDVDFramePalette_triggered(); break;
        case 24: _t->moveAllCaptions_triggered(); break;
        case 25: _t->resetCropOffset_triggered(); break;
        case 26: _t->on_subtitleLanguageComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->on_subtitleNumberComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->openConversionSettings(); break;
        case 29: _t->on_paletteComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->on_filterComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 31: _t->on_hiMedThresholdComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 32: _t->on_medLowThresholdComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->on_alphaThresholdComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 34: _t->on_outputFormatComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BDSup2Sub::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BDSup2Sub::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_BDSup2Sub,
      qt_meta_data_BDSup2Sub, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BDSup2Sub::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BDSup2Sub::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BDSup2Sub::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BDSup2Sub))
        return static_cast<void*>(const_cast< BDSup2Sub*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int BDSup2Sub::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 35)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 35;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
