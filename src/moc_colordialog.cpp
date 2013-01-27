/****************************************************************************
** Meta object code from reading C++ file 'colordialog.h'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "colordialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colordialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ColorDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   52,   58,   58, 0x08,
      59,   58,   58,   58, 0x08,
      90,   58,   58,   58, 0x08,
     130,   58,   58,   58, 0x08,
     161,   58,   58,   58, 0x08,
     192,   58,   58,   58, 0x08,
     218,   58,   58,   58, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ColorDialog[] = {
    "ColorDialog\0on_colorList_doubleClicked(QModelIndex)\0"
    "index\0\0on_changeColorButton_clicked()\0"
    "on_restoreDefaultColorsButton_clicked()\0"
    "on_savePaletteButton_clicked()\0"
    "on_loadPaletteButton_clicked()\0"
    "on_cancelButton_clicked()\0"
    "on_okButton_clicked()\0"
};

void ColorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ColorDialog *_t = static_cast<ColorDialog *>(_o);
        switch (_id) {
        case 0: _t->on_colorList_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->on_changeColorButton_clicked(); break;
        case 2: _t->on_restoreDefaultColorsButton_clicked(); break;
        case 3: _t->on_savePaletteButton_clicked(); break;
        case 4: _t->on_loadPaletteButton_clicked(); break;
        case 5: _t->on_cancelButton_clicked(); break;
        case 6: _t->on_okButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ColorDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ColorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ColorDialog,
      qt_meta_data_ColorDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColorDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorDialog))
        return static_cast<void*>(const_cast< ColorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ColorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
