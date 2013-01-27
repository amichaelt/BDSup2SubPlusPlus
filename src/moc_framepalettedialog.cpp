/****************************************************************************
** Meta object code from reading C++ file 'framepalettedialog.h'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "framepalettedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'framepalettedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FramePaletteDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   62,   68,   68, 0x08,
      69,   62,   68,   68, 0x08,
     112,   62,   68,   68, 0x08,
     155,   62,   68,   68, 0x08,
     198,   62,   68,   68, 0x08,
     241,   62,   68,   68, 0x08,
     284,   62,   68,   68, 0x08,
     327,   62,   68,   68, 0x08,
     370,   68,   68,   68, 0x08,
     398,   68,   68,   68, 0x08,
     424,   68,   68,   68, 0x08,
     450,   68,   68,   68, 0x08,
     475,   68,   68,   68, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FramePaletteDialog[] = {
    "FramePaletteDialog\0"
    "on_color1ComboBox_currentIndexChanged(int)\0"
    "index\0\0on_color2ComboBox_currentIndexChanged(int)\0"
    "on_color3ComboBox_currentIndexChanged(int)\0"
    "on_color4ComboBox_currentIndexChanged(int)\0"
    "on_alpha1ComboBox_currentIndexChanged(int)\0"
    "on_alpha2ComboBox_currentIndexChanged(int)\0"
    "on_alpha3ComboBox_currentIndexChanged(int)\0"
    "on_alpha4ComboBox_currentIndexChanged(int)\0"
    "on_resetAllButton_clicked()\0"
    "on_setAllButton_clicked()\0"
    "on_cancelButton_clicked()\0"
    "on_resetButton_clicked()\0on_okButton_clicked()\0"
};

void FramePaletteDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FramePaletteDialog *_t = static_cast<FramePaletteDialog *>(_o);
        switch (_id) {
        case 0: _t->on_color1ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_color2ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_color3ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_color4ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_alpha1ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_alpha2ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_alpha3ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_alpha4ComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_resetAllButton_clicked(); break;
        case 9: _t->on_setAllButton_clicked(); break;
        case 10: _t->on_cancelButton_clicked(); break;
        case 11: _t->on_resetButton_clicked(); break;
        case 12: _t->on_okButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FramePaletteDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FramePaletteDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FramePaletteDialog,
      qt_meta_data_FramePaletteDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FramePaletteDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FramePaletteDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FramePaletteDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FramePaletteDialog))
        return static_cast<void*>(const_cast< FramePaletteDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int FramePaletteDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
