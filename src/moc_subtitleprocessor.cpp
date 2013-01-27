/****************************************************************************
** Meta object code from reading C++ file 'subtitleprocessor.h'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Subtitles/subtitleprocessor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'subtitleprocessor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SubtitleProcessor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   46,   55,   55, 0x05,
      56,   46,   55,   55, 0x05,
      92,  127,   55,   55, 0x05,
     135,  167,   55,   55, 0x05,
     173,  211,   55,   55, 0x05,
     219,  252,   55,   55, 0x05,
     264,  252,   55,   55, 0x05,
     297,  252,   55,   55, 0x05,
     322,  341,   55,   55, 0x05,
     349,  341,   55,   55, 0x05,

 // slots: signature, parameters, type, tag, flags
     370,  394,   55,   55, 0x0a,
     406,  434,   55,   55, 0x0a,
     450,   55,   55,   55, 0x0a,
     471,   55,   55,   55, 0x0a,
     494,   55,   55,   55, 0x0a,
     504,  341,   55,   55, 0x0a,
     519,  341,   55,   55, 0x0a,
     535,  341,   55,   55, 0x0a,
     555,  341,   55,   55, 0x0a,
     577,  341,   55,   55, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SubtitleProcessor[] = {
    "SubtitleProcessor\0windowTitleChanged(QString)\0"
    "newTitle\0\0progressDialogTitleChanged(QString)\0"
    "progressDialogTextChanged(QString)\0"
    "newText\0progressDialogValueChanged(int)\0"
    "value\0progressDialogVisibilityChanged(bool)\0"
    "visible\0loadingSubtitleFinished(QString)\0"
    "errorString\0writingSubtitleFinished(QString)\0"
    "moveAllFinished(QString)\0printText(QString)\0"
    "message\0addLanguage(QString)\0"
    "setMaxProgress(quint64)\0maxProgress\0"
    "setCurrentProgress(quint64)\0currentProgress\0"
    "readSubtitleStream()\0createSubtitleStream()\0"
    "moveAll()\0print(QString)\0printX(QString)\0"
    "printError(QString)\0printWarning(QString)\0"
    "onLanguageRead(QString)\0"
};

void SubtitleProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SubtitleProcessor *_t = static_cast<SubtitleProcessor *>(_o);
        switch (_id) {
        case 0: _t->windowTitleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->progressDialogTitleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->progressDialogTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->progressDialogValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->progressDialogVisibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->loadingSubtitleFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->writingSubtitleFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->moveAllFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->printText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->addLanguage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->setMaxProgress((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 11: _t->setCurrentProgress((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 12: _t->readSubtitleStream(); break;
        case 13: _t->createSubtitleStream(); break;
        case 14: _t->moveAll(); break;
        case 15: _t->print((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->printX((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->printError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->printWarning((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->onLanguageRead((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SubtitleProcessor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SubtitleProcessor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SubtitleProcessor,
      qt_meta_data_SubtitleProcessor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SubtitleProcessor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SubtitleProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SubtitleProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SubtitleProcessor))
        return static_cast<void*>(const_cast< SubtitleProcessor*>(this));
    return QObject::qt_metacast(_clname);
}

int SubtitleProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void SubtitleProcessor::windowTitleChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SubtitleProcessor::progressDialogTitleChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SubtitleProcessor::progressDialogTextChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SubtitleProcessor::progressDialogValueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SubtitleProcessor::progressDialogVisibilityChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SubtitleProcessor::loadingSubtitleFinished(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void SubtitleProcessor::writingSubtitleFinished(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void SubtitleProcessor::moveAllFinished(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void SubtitleProcessor::printText(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void SubtitleProcessor::addLanguage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
