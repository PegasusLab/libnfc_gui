/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed 14. Jul 13:39:34 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label_Uid;
    QLineEdit *lineEdit_Uid;
    QLineEdit *lineEdit_Card;
    QLabel *label_Card;
    QPushButton *pushButton_Connect;
    QLabel *label_Status;
    QLabel *label_Reader;
    QLineEdit *lineEdit_Reader;
    QTableWidget *tableWidget_Memory;
    QGroupBox *groupBox_ClassicCommands;
    QPushButton *pushButton_AddKey;
    QPushButton *pushButton_RemKey;
    QGroupBox *groupBox_Authentication;
    QRadioButton *radioButton_UseKeyB;
    QRadioButton *radioButton_UseKeyA;
    QLabel *label_Auth;
    QLineEdit *lineEdit_AddKey;
    QLabel *label_AuthKey;
    QListWidget *listWidget_keys;
    QPushButton *pushButton_moveUp;
    QPushButton *pushButton_moveDown;
    QCheckBox *checkBox_loop;
    QCheckBox *checkBox_ask;
    QCheckBox *checkBox_UseSelectedKeys;
    QCheckBox *checkBox;
    QPushButton *pushButton;
    QTextEdit *textEdit_status;
    QPushButton *pushButton_ClearMemory;
    QPushButton *pushButton_RefreshMem;
    QPushButton *pushButton_DumpMemory;
    QPushButton *pushButton_WriteFrmDump;
    QPushButton *pushButton_StopListening;
    QPushButton *pushButton_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(1059, 604);
        MainWindow->setMouseTracking(false);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label_Uid = new QLabel(centralwidget);
        label_Uid->setObjectName(QString::fromUtf8("label_Uid"));
        label_Uid->setGeometry(QRect(20, 50, 31, 20));
        lineEdit_Uid = new QLineEdit(centralwidget);
        lineEdit_Uid->setObjectName(QString::fromUtf8("lineEdit_Uid"));
        lineEdit_Uid->setEnabled(true);
        lineEdit_Uid->setGeometry(QRect(60, 50, 131, 20));
        lineEdit_Uid->setReadOnly(true);
        lineEdit_Card = new QLineEdit(centralwidget);
        lineEdit_Card->setObjectName(QString::fromUtf8("lineEdit_Card"));
        lineEdit_Card->setGeometry(QRect(290, 50, 141, 20));
        lineEdit_Card->setReadOnly(true);
        label_Card = new QLabel(centralwidget);
        label_Card->setObjectName(QString::fromUtf8("label_Card"));
        label_Card->setGeometry(QRect(210, 50, 71, 16));
        pushButton_Connect = new QPushButton(centralwidget);
        pushButton_Connect->setObjectName(QString::fromUtf8("pushButton_Connect"));
        pushButton_Connect->setGeometry(QRect(440, 20, 81, 21));
        label_Status = new QLabel(centralwidget);
        label_Status->setObjectName(QString::fromUtf8("label_Status"));
        label_Status->setGeometry(QRect(660, 420, 46, 16));
        label_Reader = new QLabel(centralwidget);
        label_Reader->setObjectName(QString::fromUtf8("label_Reader"));
        label_Reader->setGeometry(QRect(10, 20, 46, 13));
        lineEdit_Reader = new QLineEdit(centralwidget);
        lineEdit_Reader->setObjectName(QString::fromUtf8("lineEdit_Reader"));
        lineEdit_Reader->setGeometry(QRect(60, 20, 371, 20));
        lineEdit_Reader->setReadOnly(true);
        tableWidget_Memory = new QTableWidget(centralwidget);
        tableWidget_Memory->setObjectName(QString::fromUtf8("tableWidget_Memory"));
        tableWidget_Memory->setGeometry(QRect(10, 80, 641, 501));
        tableWidget_Memory->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget_Memory->horizontalHeader()->setDefaultSectionSize(100);
        tableWidget_Memory->verticalHeader()->setVisible(false);
        tableWidget_Memory->verticalHeader()->setDefaultSectionSize(18);
        groupBox_ClassicCommands = new QGroupBox(centralwidget);
        groupBox_ClassicCommands->setObjectName(QString::fromUtf8("groupBox_ClassicCommands"));
        groupBox_ClassicCommands->setGeometry(QRect(660, 90, 391, 321));
        pushButton_AddKey = new QPushButton(groupBox_ClassicCommands);
        pushButton_AddKey->setObjectName(QString::fromUtf8("pushButton_AddKey"));
        pushButton_AddKey->setGeometry(QRect(190, 80, 91, 31));
        pushButton_RemKey = new QPushButton(groupBox_ClassicCommands);
        pushButton_RemKey->setObjectName(QString::fromUtf8("pushButton_RemKey"));
        pushButton_RemKey->setGeometry(QRect(190, 110, 91, 31));
        groupBox_Authentication = new QGroupBox(groupBox_ClassicCommands);
        groupBox_Authentication->setObjectName(QString::fromUtf8("groupBox_Authentication"));
        groupBox_Authentication->setGeometry(QRect(190, 210, 181, 51));
        radioButton_UseKeyB = new QRadioButton(groupBox_Authentication);
        radioButton_UseKeyB->setObjectName(QString::fromUtf8("radioButton_UseKeyB"));
        radioButton_UseKeyB->setGeometry(QRect(90, 30, 71, 18));
        radioButton_UseKeyA = new QRadioButton(groupBox_Authentication);
        radioButton_UseKeyA->setObjectName(QString::fromUtf8("radioButton_UseKeyA"));
        radioButton_UseKeyA->setGeometry(QRect(10, 30, 71, 18));
        label_Auth = new QLabel(groupBox_ClassicCommands);
        label_Auth->setObjectName(QString::fromUtf8("label_Auth"));
        label_Auth->setGeometry(QRect(10, 30, 131, 16));
        lineEdit_AddKey = new QLineEdit(groupBox_ClassicCommands);
        lineEdit_AddKey->setObjectName(QString::fromUtf8("lineEdit_AddKey"));
        lineEdit_AddKey->setGeometry(QRect(190, 50, 91, 31));
        lineEdit_AddKey->setMaxLength(12);
        label_AuthKey = new QLabel(groupBox_ClassicCommands);
        label_AuthKey->setObjectName(QString::fromUtf8("label_AuthKey"));
        label_AuthKey->setGeometry(QRect(190, 30, 171, 16));
        listWidget_keys = new QListWidget(groupBox_ClassicCommands);
        listWidget_keys->setObjectName(QString::fromUtf8("listWidget_keys"));
        listWidget_keys->setGeometry(QRect(10, 50, 171, 261));
        listWidget_keys->setSelectionMode(QAbstractItemView::ExtendedSelection);
        listWidget_keys->setSelectionBehavior(QAbstractItemView::SelectItems);
        pushButton_moveUp = new QPushButton(groupBox_ClassicCommands);
        pushButton_moveUp->setObjectName(QString::fromUtf8("pushButton_moveUp"));
        pushButton_moveUp->setGeometry(QRect(190, 140, 91, 31));
        pushButton_moveDown = new QPushButton(groupBox_ClassicCommands);
        pushButton_moveDown->setObjectName(QString::fromUtf8("pushButton_moveDown"));
        pushButton_moveDown->setGeometry(QRect(190, 170, 91, 31));
        checkBox_loop = new QCheckBox(groupBox_ClassicCommands);
        checkBox_loop->setObjectName(QString::fromUtf8("checkBox_loop"));
        checkBox_loop->setGeometry(QRect(190, 260, 191, 18));
        checkBox_ask = new QCheckBox(groupBox_ClassicCommands);
        checkBox_ask->setObjectName(QString::fromUtf8("checkBox_ask"));
        checkBox_ask->setGeometry(QRect(190, 280, 191, 18));
        checkBox_UseSelectedKeys = new QCheckBox(groupBox_ClassicCommands);
        checkBox_UseSelectedKeys->setObjectName(QString::fromUtf8("checkBox_UseSelectedKeys"));
        checkBox_UseSelectedKeys->setGeometry(QRect(190, 300, 201, 18));
        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(440, 50, 101, 18));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(800, 60, 151, 23));
        textEdit_status = new QTextEdit(centralwidget);
        textEdit_status->setObjectName(QString::fromUtf8("textEdit_status"));
        textEdit_status->setGeometry(QRect(660, 440, 391, 141));
        textEdit_status->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        textEdit_status->setLineWrapMode(QTextEdit::NoWrap);
        pushButton_ClearMemory = new QPushButton(centralwidget);
        pushButton_ClearMemory->setObjectName(QString::fromUtf8("pushButton_ClearMemory"));
        pushButton_ClearMemory->setEnabled(true);
        pushButton_ClearMemory->setGeometry(QRect(800, 40, 151, 23));
        pushButton_RefreshMem = new QPushButton(centralwidget);
        pushButton_RefreshMem->setObjectName(QString::fromUtf8("pushButton_RefreshMem"));
        pushButton_RefreshMem->setGeometry(QRect(660, 20, 131, 23));
        pushButton_DumpMemory = new QPushButton(centralwidget);
        pushButton_DumpMemory->setObjectName(QString::fromUtf8("pushButton_DumpMemory"));
        pushButton_DumpMemory->setGeometry(QRect(660, 40, 131, 23));
        pushButton_WriteFrmDump = new QPushButton(centralwidget);
        pushButton_WriteFrmDump->setObjectName(QString::fromUtf8("pushButton_WriteFrmDump"));
        pushButton_WriteFrmDump->setGeometry(QRect(660, 60, 131, 23));
        pushButton_StopListening = new QPushButton(centralwidget);
        pushButton_StopListening->setObjectName(QString::fromUtf8("pushButton_StopListening"));
        pushButton_StopListening->setGeometry(QRect(530, 20, 121, 23));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(800, 20, 151, 23));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Mifare Diagnostic Tool", 0, QApplication::UnicodeUTF8));
        label_Uid->setText(QApplication::translate("MainWindow", "UID", 0, QApplication::UnicodeUTF8));
        lineEdit_Uid->setText(QString());
        label_Card->setText(QApplication::translate("MainWindow", "Card Type", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_Connect->setToolTip(QApplication::translate("MainWindow", "Connects to the first available and supported device", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_Connect->setText(QApplication::translate("MainWindow", "&Connect", 0, QApplication::UnicodeUTF8));
        label_Status->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));
        label_Reader->setText(QApplication::translate("MainWindow", "Reader", 0, QApplication::UnicodeUTF8));
        groupBox_ClassicCommands->setTitle(QApplication::translate("MainWindow", "Mifare Classic Options", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_AddKey->setToolTip(QApplication::translate("MainWindow", "add key to  list", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_AddKey->setText(QApplication::translate("MainWindow", "Add Key", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_RemKey->setToolTip(QApplication::translate("MainWindow", "Removes key from list", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_RemKey->setText(QApplication::translate("MainWindow", "Remove Key", 0, QApplication::UnicodeUTF8));
        groupBox_Authentication->setTitle(QApplication::translate("MainWindow", "Auth Mode", 0, QApplication::UnicodeUTF8));
        radioButton_UseKeyB->setText(QApplication::translate("MainWindow", "KeyB", 0, QApplication::UnicodeUTF8));
        radioButton_UseKeyA->setText(QApplication::translate("MainWindow", "KeyA", 0, QApplication::UnicodeUTF8));
        label_Auth->setText(QApplication::translate("MainWindow", "Auth Keys List", 0, QApplication::UnicodeUTF8));
        lineEdit_AddKey->setText(QString());
        label_AuthKey->setText(QApplication::translate("MainWindow", "6byte Hex/Ascii Key", 0, QApplication::UnicodeUTF8));
        pushButton_moveUp->setText(QApplication::translate("MainWindow", "Key Up", 0, QApplication::UnicodeUTF8));
        pushButton_moveDown->setText(QApplication::translate("MainWindow", "Key Down", 0, QApplication::UnicodeUTF8));
        checkBox_loop->setText(QApplication::translate("MainWindow", "Do&n't Loop keys list", 0, QApplication::UnicodeUTF8));
        checkBox_ask->setText(QApplication::translate("MainWindow", "Ask when none worked", 0, QApplication::UnicodeUTF8));
        checkBox_UseSelectedKeys->setText(QApplication::translate("MainWindow", "Use Selected keys only", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("MainWindow", "&Hex Mode", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "E&xit ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_ClearMemory->setToolTip(QApplication::translate("MainWindow", "Resets Memory to Factory Values, Trailers will be set to transport configuration 000000000000 and FFFFFFFFFFFF", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_ClearMemory->setText(QApplication::translate("MainWindow", "Reset Tag2Factory", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_RefreshMem->setToolTip(QApplication::translate("MainWindow", "Refresh Tag Memory", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_RefreshMem->setText(QApplication::translate("MainWindow", "Refresh Mem", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_DumpMemory->setToolTip(QApplication::translate("MainWindow", "Dumps Tag Memory to file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_DumpMemory->setText(QApplication::translate("MainWindow", "Dump Mem2File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_WriteFrmDump->setToolTip(QApplication::translate("MainWindow", "Writes memory From Dump File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_WriteFrmDump->setText(QApplication::translate("MainWindow", "Write from Dump", 0, QApplication::UnicodeUTF8));
        pushButton_StopListening->setText(QApplication::translate("MainWindow", "&Stop Listening", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MainWindow", "Save Keys2File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
