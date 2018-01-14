#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//qt libraries
#include <QModelIndex>
#include <QTextStream>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFile>
#include <QInputDialog>
#include <QTimer>
#include <QTableWidget>
#include <QFileDialog>
//c libraries
#include <targetver.h>
#include <tchar.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif // HAVE_CONFIG_H
//libnfc libraries!
#include "nfc/nfc-types.h"
#include "nfc/nfc.h"
#include "nfc/mifareultag.h"
#include "nfc/bitutils.h"
#include "mifaretag.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void update_status(const QString & text);
    void get_card_type(byte_t* atqa,byte_t* sak);
    void print_success_or_failure(bool bFailure, int block);
    void update_keys();
    bool read_classic_card2(nfc_device_t* pnd,int sectorblock,int block_stop);
    bool read_ultralight_card(nfc_device_t* pnd,unsigned int page_start,unsigned int page_end);
    bool read_classic_card(nfc_device_t* pnd,int sectorblock,int block_stop);
    bool write_classic_card(nfc_device_t* pnd,int block,byte_t* data,int length);
    bool write_ultralight_card(nfc_device_t* pnd,byte_t* content,unsigned page_start,unsigned page_end,int len);
    bool authenticate(nfc_device_t* pnd,uint32_t uiBlock);
    void disconnect_reader();
    bool select_tag();
    void parse_trailer(byte_t* data);
    void parse_lockbytes(byte_t* data);
    void initialize_device();
    void connect_reader();
    void  savekeys(nfc_device_t* pnd);
    QString a(byte_t* bytes,int len);
    void read();
    void clear();
    bool valids(QString s);
    void moveKeyUp();
    void moveKeyDown();
    void update_key_file();
    bool valid_key(QString s);
    void add_key(QString str);
protected:
    void changeEvent(QEvent *e);
    void polishEvent();

private:
  Ui::MainWindow *ui;
  QTimer* iTimer;
  QTimer* timerWait;
private slots:
    void on_pushButton_WriteFrmDump_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_StopListening_clicked();
    void on_pushButton_DumpMemory_clicked();
    void on_pushButton_RefreshMem_clicked();
    void on_checkBox_UseSelectedKeys_clicked();
    void on_pushButton_ClearMemory_clicked();
    void on_pushButton_moveDown_clicked();
    void on_pushButton_moveUp_clicked();
    void on_checkBox_loop_clicked();
    void timerupdate();
    void on_tableWidget_Memory_cellDoubleClicked(int row, int column);
    void on_listWidget_keys_itemClicked(QListWidgetItem* item);
      void on_pushButton_clicked();
    void closeEvent(QCloseEvent *event);
    void on_pushButton_RemKey_clicked();
    void on_pushButton_AddKey_clicked();
    void on_radioButton_UseKeyB_clicked();
    void on_radioButton_UseKeyA_clicked();
    void on_checkBox_clicked();
    void on_tableWidget_Memory_cellClicked(int row, int column);
    void on_pushButton_Connect_clicked();
    void showEvent(QShowEvent * event);
    void on_tableWidget_Memory_cellChanged(int row, int column);
    void TimeOut(void);
    void keyPressEvent(QKeyEvent* e);
};


#endif // MAINWINDOW_H
