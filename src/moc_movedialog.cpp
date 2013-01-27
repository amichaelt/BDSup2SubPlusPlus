/****************************************************************************
** Meta object code from reading C++ file 'movedialog.h'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "movedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'movedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MoveDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   39,   39,   39, 0x08,
      40,   39,   39,   39, 0x08,
      64,   39,   39,   39, 0x08,
      92,   39,   39,   39, 0x08,
     118,   39,   39,   39, 0x08,
     145,   39,   39,   39, 0x08,
     177,   39,   39,   39, 0x08,
     209,   39,   39,   39, 0x08,
     241,   39,   39,   39, 0x08,
     279,   39,   39,   39, 0x08,
     321,   39,   39,   39, 0x08,
     354,   39,   39,   39, 0x08,
     388,   39,   39,   39, 0x08,
     425,   39,   39,   39, 0x08,
     463,   39,   39,   39, 0x08,
     505,   39,   39,   39, 0x08,
     546,   39,   39,   39, 0x08,
     588,  628,   39,   39, 0x08,
     633,  628,   39,   39, 0x08,
     673,  628,   39,   39, 0x08,
     717,  628,   39,   39, 0x08,
     761,   39,   39,   39, 0x08,
     798,   39,   39,   39, 0x08,
     835,   39,   39,   39, 0x08,
     876,   39,   39,   39, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MoveDialog[] = {
    "MoveDialog\0on_previousButton_clicked()\0"
    "\0on_nextButton_clicked()\0"
    "on_cropBarsButton_clicked()\0"
    "on_cancelButton_clicked()\0"
    "on_moveAllButton_clicked()\0"
    "on_aspectRatio1Button_clicked()\0"
    "on_aspectRatio2Button_clicked()\0"
    "on_aspectRatio3Button_clicked()\0"
    "on_keepXPositionRadioButton_clicked()\0"
    "on_moveFromXPositionRadioButton_clicked()\0"
    "on_moveLeftRadioButton_clicked()\0"
    "on_moveRightRadioButton_clicked()\0"
    "on_moveToCenterRadioButton_clicked()\0"
    "on_keepYPositionRadioButton_clicked()\0"
    "on_moveFromYPositionRadioButton_clicked()\0"
    "on_moveInsideBoundsRadioButton_clicked()\0"
    "on_moveOutsideBoundsRadioButton_clicked()\0"
    "on_xOffsetLineEdit_textChanged(QString)\0"
    "arg1\0on_yOffsetLineEdit_textChanged(QString)\0"
    "on_aspectRatioLineEdit_textChanged(QString)\0"
    "on_cropOffsetYLineEdit_textChanged(QString)\0"
    "on_xOffsetLineEdit_editingFinished()\0"
    "on_yOffsetLineEdit_editingFinished()\0"
    "on_aspectRatioLineEdit_editingFinished()\0"
    "on_cropOffsetYLineEdit_editingFinished()\0"
};

void MoveDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MoveDialog *_t = static_cast<MoveDialog *>(_o);
        switch (_id) {
        case 0: _t->on_previousButton_clicked(); break;
        case 1: _t->on_nextButton_clicked(); break;
        case 2: _t->on_cropBarsButton_clicked(); break;
        case 3: _t->on_cancelButton_clicked(); break;
        case 4: _t->on_moveAllButton_clicked(); break;
        case 5: _t->on_aspectRatio1Button_clicked(); break;
        case 6: _t->on_aspectRatio2Button_clicked(); break;
        case 7: _t->on_aspectRatio3Button_clicked(); break;
        case 8: _t->on_keepXPositionRadioButton_clicked(); break;
        case 9: _t->on_moveFromXPositionRadioButton_clicked(); break;
        case 10: _t->on_moveLeftRadioButton_clicked(); break;
        case 11: _t->on_moveRightRadioButton_clicked(); break;
        case 12: _t->on_moveToCenterRadioButton_clicked(); break;
        case 13: _t->on_keepYPositionRadioButton_clicked(); break;
        case 14: _t->on_moveFromYPositionRadioButton_clicked(); break;
        case 15: _t->on_moveInsideBoundsRadioButton_clicked(); break;
        case 16: _t->on_moveOutsideBoundsRadioButton_clicked(); break;
        case 17: _t->on_xOffsetLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->on_yOffsetLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->on_aspectRatioLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: _t->on_cropOffsetYLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: _t->on_xOffsetLineEdit_editingFinished(); break;
        case 22: _t->on_yOffsetLineEdit_editingFinished(); break;
        case 23: _t->on_aspectRatioLineEdit_editingFinished(); break;
        case 24: _t->on_cropOffsetYLineEdit_editingFinished(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MoveDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MoveDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MoveDialog,
      qt_meta_data_MoveDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MoveDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MoveDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MoveDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MoveDialog))
        return static_cast<void*>(const_cast< MoveDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MoveDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
