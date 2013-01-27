/****************************************************************************
** Meta object code from reading C++ file 'subdvd.h'
**
** Created: Sat Jan 26 17:55:59 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Subtitles/subdvd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'subdvd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SubDVD[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,   35,   47,   47, 0x05,
      48,   80,   47,   47, 0x05,
      96,  117,   47,   47, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_SubDVD[] = {
    "SubDVD\0maxProgressChanged(quint64)\0"
    "maxProgress\0\0currentProgressChanged(quint64)\0"
    "currentProgress\0addLanguage(QString)\0"
    "message\0"
};

void SubDVD::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SubDVD *_t = static_cast<SubDVD *>(_o);
        switch (_id) {
        case 0: _t->maxProgressChanged((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 1: _t->currentProgressChanged((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 2: _t->addLanguage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SubDVD::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SubDVD::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SubDVD,
      qt_meta_data_SubDVD, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SubDVD::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SubDVD::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SubDVD::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SubDVD))
        return static_cast<void*>(const_cast< SubDVD*>(this));
    if (!strcmp(_clname, "Substream"))
        return static_cast< Substream*>(const_cast< SubDVD*>(this));
    if (!strcmp(_clname, "SubstreamDVD"))
        return static_cast< SubstreamDVD*>(const_cast< SubDVD*>(this));
    return QObject::qt_metacast(_clname);
}

int SubDVD::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void SubDVD::maxProgressChanged(quint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SubDVD::currentProgressChanged(quint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SubDVD::addLanguage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
