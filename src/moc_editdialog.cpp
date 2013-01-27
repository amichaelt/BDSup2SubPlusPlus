/****************************************************************************
** Meta object code from reading C++ file 'editdialog.h'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "editdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   34,   34,   34, 0x08,
      35,   34,   34,   34, 0x08,
      58,   34,   34,   34, 0x08,
      84,   34,   34,   34, 0x08,
     107,   34,   34,   34, 0x08,
     133,   34,   34,   34, 0x08,
     166,   34,   34,   34, 0x08,
     200,   34,   34,   34, 0x08,
     239,   34,   34,   34, 0x08,
     265,   34,   34,   34, 0x08,
     289,   34,   34,   34, 0x08,
     311,  344,   34,   34, 0x08,
     352,  344,   34,   34, 0x08,
     391,  427,   34,   34, 0x08,
     433,  427,   34,   34, 0x08,
     471,  513,   34,   34, 0x08,
     518,  513,   34,   34, 0x08,
     558,  513,   34,   34, 0x08,
     599,  513,   34,   34, 0x08,
     639,  513,   34,   34, 0x08,
     679,   34,   34,   34, 0x08,
     718,   34,   34,   34, 0x08,
     755,   34,   34,   34, 0x08,
     793,   34,   34,   34, 0x08,
     830,   34,   34,   34, 0x08,
     867,  894,   34,   34, 0x08,
     909,   34,   34,   34, 0x08,
     942,   34,   34,   34, 0x08,
     971,   34,   34,   34, 0x08,
     999,   34,   34,   34, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EditDialog[] = {
    "EditDialog\0on_minButton_clicked()\0\0"
    "on_maxButton_clicked()\0on_centerButton_clicked()\0"
    "on_topButton_clicked()\0on_bottomButton_clicked()\0"
    "on_addErasePatchButton_clicked()\0"
    "on_undoErasePatchButton_clicked()\0"
    "on_undoAllErasePatchesButton_clicked()\0"
    "on_cancelButton_clicked()\0"
    "on_saveButton_clicked()\0on_okButton_clicked()\0"
    "on_excludeCheckBox_toggled(bool)\0"
    "checked\0on_forcedCaptionCheckBox_toggled(bool)\0"
    "on_verticalSlider_valueChanged(int)\0"
    "value\0on_horizontalSlider_valueChanged(int)\0"
    "on_startTimeLineEdit_textChanged(QString)\0"
    "arg1\0on_endTimeLineEdit_textChanged(QString)\0"
    "on_durationLineEdit_textChanged(QString)\0"
    "on_xOffsetLineEdit_textChanged(QString)\0"
    "on_yOffsetLineEdit_textChanged(QString)\0"
    "on_startTimeLineEdit_editingFinished()\0"
    "on_endTimeLineEdit_editingFinished()\0"
    "on_durationLineEdit_editingFinished()\0"
    "on_xOffsetLineEdit_editingFinished()\0"
    "on_yOffsetLineEdit_editingFinished()\0"
    "onSelectionPerformed(bool)\0validSelection\0"
    "on_storePreviousButton_clicked()\0"
    "on_storeNextButton_clicked()\0"
    "on_previousButton_clicked()\0"
    "on_nextButton_clicked()\0"
};

void EditDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditDialog *_t = static_cast<EditDialog *>(_o);
        switch (_id) {
        case 0: _t->on_minButton_clicked(); break;
        case 1: _t->on_maxButton_clicked(); break;
        case 2: _t->on_centerButton_clicked(); break;
        case 3: _t->on_topButton_clicked(); break;
        case 4: _t->on_bottomButton_clicked(); break;
        case 5: _t->on_addErasePatchButton_clicked(); break;
        case 6: _t->on_undoErasePatchButton_clicked(); break;
        case 7: _t->on_undoAllErasePatchesButton_clicked(); break;
        case 8: _t->on_cancelButton_clicked(); break;
        case 9: _t->on_saveButton_clicked(); break;
        case 10: _t->on_okButton_clicked(); break;
        case 11: _t->on_excludeCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->on_forcedCaptionCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->on_verticalSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->on_horizontalSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->on_startTimeLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->on_endTimeLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->on_durationLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->on_xOffsetLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->on_yOffsetLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: _t->on_startTimeLineEdit_editingFinished(); break;
        case 21: _t->on_endTimeLineEdit_editingFinished(); break;
        case 22: _t->on_durationLineEdit_editingFinished(); break;
        case 23: _t->on_xOffsetLineEdit_editingFinished(); break;
        case 24: _t->on_yOffsetLineEdit_editingFinished(); break;
        case 25: _t->onSelectionPerformed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: _t->on_storePreviousButton_clicked(); break;
        case 27: _t->on_storeNextButton_clicked(); break;
        case 28: _t->on_previousButton_clicked(); break;
        case 29: _t->on_nextButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EditDialog,
      qt_meta_data_EditDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditDialog))
        return static_cast<void*>(const_cast< EditDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int EditDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
