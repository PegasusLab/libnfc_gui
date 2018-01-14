/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Sun Jan 14 18:11:21 2018
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Libnfc_gui/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      49,   11,   11,   11, 0x08,
      75,   11,   11,   11, 0x08,
     113,   11,   11,   11, 0x08,
     148,   11,   11,   11, 0x08,
     183,   11,   11,   11, 0x08,
     221,   11,   11,   11, 0x08,
     257,   11,   11,   11, 0x08,
     290,   11,   11,   11, 0x08,
     321,   11,   11,   11, 0x08,
     348,   11,   11,   11, 0x08,
     373,  362,   11,   11, 0x08,
     427,  422,   11,   11, 0x08,
     476,   11,   11,   11, 0x08,
     506,  500,   11,   11, 0x08,
     531,   11,   11,   11, 0x08,
     562,   11,   11,   11, 0x08,
     593,   11,   11,   11, 0x08,
     626,   11,   11,   11, 0x08,
     659,   11,   11,   11, 0x08,
     681,  362,   11,   11, 0x08,
     724,   11,   11,   11, 0x08,
     756,  500,   11,   11, 0x08,
     779,  362,   11,   11, 0x08,
     822,   11,   11,   11, 0x08,
     834,  832,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_pushButton_WriteFrmDump_clicked()\0"
    "on_pushButton_2_clicked()\0"
    "on_pushButton_StopListening_clicked()\0"
    "on_pushButton_DumpMemory_clicked()\0"
    "on_pushButton_RefreshMem_clicked()\0"
    "on_checkBox_UseSelectedKeys_clicked()\0"
    "on_pushButton_ClearMemory_clicked()\0"
    "on_pushButton_moveDown_clicked()\0"
    "on_pushButton_moveUp_clicked()\0"
    "on_checkBox_loop_clicked()\0timerupdate()\0"
    "row,column\0on_tableWidget_Memory_cellDoubleClicked(int,int)\0"
    "item\0on_listWidget_keys_itemClicked(QListWidgetItem*)\0"
    "on_pushButton_clicked()\0event\0"
    "closeEvent(QCloseEvent*)\0"
    "on_pushButton_RemKey_clicked()\0"
    "on_pushButton_AddKey_clicked()\0"
    "on_radioButton_UseKeyB_clicked()\0"
    "on_radioButton_UseKeyA_clicked()\0"
    "on_checkBox_clicked()\0"
    "on_tableWidget_Memory_cellClicked(int,int)\0"
    "on_pushButton_Connect_clicked()\0"
    "showEvent(QShowEvent*)\0"
    "on_tableWidget_Memory_cellChanged(int,int)\0"
    "TimeOut()\0e\0keyPressEvent(QKeyEvent*)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_pushButton_WriteFrmDump_clicked(); break;
        case 1: on_pushButton_2_clicked(); break;
        case 2: on_pushButton_StopListening_clicked(); break;
        case 3: on_pushButton_DumpMemory_clicked(); break;
        case 4: on_pushButton_RefreshMem_clicked(); break;
        case 5: on_checkBox_UseSelectedKeys_clicked(); break;
        case 6: on_pushButton_ClearMemory_clicked(); break;
        case 7: on_pushButton_moveDown_clicked(); break;
        case 8: on_pushButton_moveUp_clicked(); break;
        case 9: on_checkBox_loop_clicked(); break;
        case 10: timerupdate(); break;
        case 11: on_tableWidget_Memory_cellDoubleClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: on_listWidget_keys_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 13: on_pushButton_clicked(); break;
        case 14: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 15: on_pushButton_RemKey_clicked(); break;
        case 16: on_pushButton_AddKey_clicked(); break;
        case 17: on_radioButton_UseKeyB_clicked(); break;
        case 18: on_radioButton_UseKeyA_clicked(); break;
        case 19: on_checkBox_clicked(); break;
        case 20: on_tableWidget_Memory_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: on_pushButton_Connect_clicked(); break;
        case 22: showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 23: on_tableWidget_Memory_cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 24: TimeOut(); break;
        case 25: keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 26;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
