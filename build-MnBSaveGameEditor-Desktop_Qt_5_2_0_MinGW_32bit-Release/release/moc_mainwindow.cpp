/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MnBSaveGameEditor/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[28];
    char stringdata[330];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 14),
QT_MOC_LITERAL(2, 26, 0),
QT_MOC_LITERAL(3, 27, 11),
QT_MOC_LITERAL(4, 39, 18),
QT_MOC_LITERAL(5, 58, 12),
QT_MOC_LITERAL(6, 71, 8),
QT_MOC_LITERAL(7, 80, 12),
QT_MOC_LITERAL(8, 93, 16),
QT_MOC_LITERAL(9, 110, 6),
QT_MOC_LITERAL(10, 117, 12),
QT_MOC_LITERAL(11, 130, 14),
QT_MOC_LITERAL(12, 145, 7),
QT_MOC_LITERAL(13, 153, 15),
QT_MOC_LITERAL(14, 169, 11),
QT_MOC_LITERAL(15, 181, 4),
QT_MOC_LITERAL(16, 186, 6),
QT_MOC_LITERAL(17, 193, 10),
QT_MOC_LITERAL(18, 204, 2),
QT_MOC_LITERAL(19, 207, 10),
QT_MOC_LITERAL(20, 218, 17),
QT_MOC_LITERAL(21, 236, 18),
QT_MOC_LITERAL(22, 255, 15),
QT_MOC_LITERAL(23, 271, 5),
QT_MOC_LITERAL(24, 277, 15),
QT_MOC_LITERAL(25, 293, 11),
QT_MOC_LITERAL(26, 305, 11),
QT_MOC_LITERAL(27, 317, 11)
    },
    "MainWindow\0BrowseSGWindow\0\0CloseBrowse\0"
    "BrowseModDirectory\0BrowseSGFile\0"
    "LoadSave\0AddTreeChild\0QTreeWidgetItem*\0"
    "parent\0QTreeWidget*\0SaveGameInfos*\0"
    "SGInfos\0RemoveTreeChild\0TryEditItem\0"
    "item\0column\0ChangeTree\0id\0ExportSave\0"
    "ConvertSaveWindow\0CloseConvertWindow\0"
    "ChangeModAIndex\0newId\0ChangeModBIndex\0"
    "ConvertSave\0GetSettings\0SetSettings\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x0a,
       3,    0,  110,    2, 0x0a,
       4,    0,  111,    2, 0x0a,
       5,    0,  112,    2, 0x0a,
       6,    0,  113,    2, 0x0a,
       7,    1,  114,    2, 0x0a,
       7,    2,  117,    2, 0x0a,
      13,    1,  122,    2, 0x0a,
      14,    2,  125,    2, 0x0a,
      14,    1,  130,    2, 0x2a,
      17,    1,  133,    2, 0x0a,
      19,    0,  136,    2, 0x0a,
      20,    0,  137,    2, 0x0a,
      21,    0,  138,    2, 0x0a,
      22,    1,  139,    2, 0x0a,
      24,    1,  142,    2, 0x0a,
      25,    0,  145,    2, 0x0a,
      26,    0,  146,    2, 0x0a,
      27,    0,  147,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 11,    9,   12,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,   15,   16,
    QMetaType::Void, 0x80000000 | 8,   15,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->BrowseSGWindow(); break;
        case 1: _t->CloseBrowse(); break;
        case 2: _t->BrowseModDirectory(); break;
        case 3: _t->BrowseSGFile(); break;
        case 4: _t->LoadSave(); break;
        case 5: _t->AddTreeChild((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->AddTreeChild((*reinterpret_cast< QTreeWidget*(*)>(_a[1])),(*reinterpret_cast< SaveGameInfos*(*)>(_a[2]))); break;
        case 7: _t->RemoveTreeChild((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->TryEditItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->TryEditItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 10: _t->ChangeTree((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->ExportSave(); break;
        case 12: _t->ConvertSaveWindow(); break;
        case 13: _t->CloseConvertWindow(); break;
        case 14: _t->ChangeModAIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->ChangeModBIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->ConvertSave(); break;
        case 17: _t->GetSettings(); break;
        case 18: _t->SetSettings(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTreeWidget* >(); break;
            }
            break;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
