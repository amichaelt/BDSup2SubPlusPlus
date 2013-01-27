/****************************************************************************
** Meta object code from reading C++ file 'editpane.h'
**
** Created: Sat Jan 26 17:55:59 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "editpane.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editpane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditPane[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,   38,   44,   44, 0x05,
      45,   70,   44,   44, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_EditPane[] = {
    "EditPane\0onMouseClicked(QMouseEvent*)\0"
    "event\0\0selectionPerformed(bool)\0"
    "validSelection\0"
};

void EditPane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditPane *_t = static_cast<EditPane *>(_o);
        switch (_id) {
        case 0: _t->onMouseClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 1: _t->selectionPerformed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditPane::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditPane::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_EditPane,
      qt_meta_data_EditPane, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditPane::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditPane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditPane::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditPane))
        return static_cast<void*>(const_cast< EditPane*>(this));
    return QLabel::qt_metacast(_clname);
}

int EditPane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void EditPane::onMouseClicked(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EditPane::selectionPerformed(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
