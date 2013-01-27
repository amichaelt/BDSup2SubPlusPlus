/****************************************************************************
** Meta object code from reading C++ file 'conversiondialog.h'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "conversiondialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'conversiondialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConversionDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   39,   39,   39, 0x08,
      40,   39,   39,   39, 0x08,
      66,   39,   39,   39, 0x08,
      91,   39,   39,   39, 0x08,
     118,   39,   39,   39, 0x08,
     143,  186,   39,   39, 0x08,
     194,  186,   39,   39, 0x08,
     240,  186,   39,   39, 0x08,
     281,  186,   39,   39, 0x08,
     312,  186,   39,   39, 0x08,
     355,  402,   39,   39, 0x08,
     408,  402,   39,   39, 0x08,
     455,  511,   39,   39, 0x08,
     516,  511,   39,   39, 0x08,
     572,  511,   39,   39, 0x08,
     611,  511,   39,   39, 0x08,
     650,  511,   39,   39, 0x08,
     688,  511,   39,   39, 0x08,
     728,  511,   39,   39, 0x08,
     780,  511,   39,   39, 0x08,
     832,   39,   39,   39, 0x08,
     868,   39,   39,   39, 0x08,
     904,   39,   39,   39, 0x08,
     939,   39,   39,   39, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ConversionDialog[] = {
    "ConversionDialog\0on_okButton_clicked()\0"
    "\0on_cancelButton_clicked()\0"
    "on_storeButton_clicked()\0"
    "on_restoreButton_clicked()\0"
    "on_resetButton_clicked()\0"
    "on_convertResolutionCheckBox_toggled(bool)\0"
    "checked\0on_applyMoveAllSettingsCheckBox_toggled(bool)\0"
    "on_changeFrameRateCheckBox_toggled(bool)\0"
    "on_scaleCheckBox_toggled(bool)\0"
    "on_fixTooShortFramesCheckBox_toggled(bool)\0"
    "on_resolutionComboBox_currentIndexChanged(int)\0"
    "index\0on_forceFlagsComboBox_currentIndexChanged(int)\0"
    "on_sourceFramerateComboBox_currentIndexChanged(QString)\0"
    "arg1\0on_targetFramerateComboBox_currentIndexChanged(QString)\0"
    "on_scaleXLineEdit_textChanged(QString)\0"
    "on_scaleYLineEdit_textChanged(QString)\0"
    "on_delayLineEdit_textChanged(QString)\0"
    "on_minTimeLineEdit_textChanged(QString)\0"
    "on_sourceFramerateComboBox_editTextChanged(QString)\0"
    "on_targetFramerateComboBox_editTextChanged(QString)\0"
    "on_scaleXLineEdit_editingFinished()\0"
    "on_scaleYLineEdit_editingFinished()\0"
    "on_delayLineEdit_editingFinished()\0"
    "on_minTimeLineEdit_editingFinished()\0"
};

void ConversionDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ConversionDialog *_t = static_cast<ConversionDialog *>(_o);
        switch (_id) {
        case 0: _t->on_okButton_clicked(); break;
        case 1: _t->on_cancelButton_clicked(); break;
        case 2: _t->on_storeButton_clicked(); break;
        case 3: _t->on_restoreButton_clicked(); break;
        case 4: _t->on_resetButton_clicked(); break;
        case 5: _t->on_convertResolutionCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_applyMoveAllSettingsCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_changeFrameRateCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_scaleCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->on_fixTooShortFramesCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->on_resolutionComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->on_forceFlagsComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->on_sourceFramerateComboBox_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->on_targetFramerateComboBox_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->on_scaleXLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->on_scaleYLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->on_delayLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->on_minTimeLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->on_sourceFramerateComboBox_editTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->on_targetFramerateComboBox_editTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: _t->on_scaleXLineEdit_editingFinished(); break;
        case 21: _t->on_scaleYLineEdit_editingFinished(); break;
        case 22: _t->on_delayLineEdit_editingFinished(); break;
        case 23: _t->on_minTimeLineEdit_editingFinished(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ConversionDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ConversionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ConversionDialog,
      qt_meta_data_ConversionDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ConversionDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ConversionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ConversionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConversionDialog))
        return static_cast<void*>(const_cast< ConversionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ConversionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
