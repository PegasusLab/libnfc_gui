#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>

#include <QKeyEvent>
#include <Qt>
#include <QList>
#include <QScrollBar>
static nfc_device_t* pnd;
static nfc_target_info_t nti;
static nfc_target_info_t last_target;
static mifare_param mp;
static mifare_tag mtDump;
static bool bUseKeyA;
static bool first_show = false;
static bool stop=false;// first MainWindowshow event
static uint32_t uiBlocks;
static int card_type=0; // int to identify card_types.. Mini,Classic.Ultralight etc
static bool connected = false;
static bool tag_found = false;
static bool hex_mode = false;
static bool dont_loop = false;

//static stop_searching = false;
static int  timercounter=10;
static int wait=0;
typedef struct {
    bool c1;
    bool c2;
    bool c3;
} AccessCondition;
static AccessCondition acs[4];
/* Some Default Keys
   
//  0x00,0x00,0x00,0x00,0x00,0x00,
//  0xff,0xff,0xff,0xff,0xff,0xff,
//  0xd3,0xf7,0xd3,0xf7,0xd3,0xf7,
//  0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,
//  0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,
//  0x4d,0x3a,0x99,0xc3,0x51,0xdd,
//  0x1a,0x98,0x2c,0x7e,0x45,0x9a,
//  0xaa,0xbb,0xcc,0xdd,0xee,0xff
*/
static byte_t last_good_key[6];
static byte_t keys[360]; // 60 keys !! if you want more resize array)

byte_t * QStr2Byte(QString data) {
 //  convert from QString to byte
  QString str = data;
  byte_t *s = NULL;
  s = (unsigned char*)qstrdup(str.toAscii().constData());
  return s;
}
QByteArray b2QByteArray(byte_t* data,int length) {
    // converts from byte_t to QByteArray byte_t is the format handled by libnfc (unsigned char )
    QByteArray str;
    str.resize(length);
    int ptr = 0;
        for(;ptr < length;ptr++)
        {
            str[ptr]= data[ptr];
        }
     return str;
//      QByteArray data = QByteArray::fromRawData(data, length);
//      QDataStream in(&data, QIODevice::ReadOnly);
//      return data;
}
QString b2QStringAscii(byte_t* data, int length)
{
    // convert from byte_t to QString. Will hold data in Ascii Format."abcdef093fmjgham"
    QString str;
    str.resize(length+1);
    int ptr=0;
        for(;ptr < length;ptr++)
        {
              str[ptr]= data[ptr];
       }
//         str.toAscii();
     return str;
}
unsigned char c2a(unsigned char c)
{
    //aid function
    if(c<=9)
    {
        return c+0x30;
    }
    return c-0x0A+'A';
}
QString b2QStringHex(byte_t* bytes, int len)
{
       // function to convert from byte_t to Qstring rt will hold data in Hex Format
    QString rt;
     for(int i=0; i<len; i++)
    {
         char l = (bytes[i]>>4)&0x0F;
         char r= bytes[i]&0x0F;
         QChar c[2] = { c2a(l),c2a(r)};
         rt += QString(c,2);
     }
     return  rt.toUpper();
 }
QString ins_spaces(QString str) {
    //to be used with hex digits "ffffffff" becomes "ff ff ff ff"
    QString x=str;
    int i=2;
    for (;i<x.size();i=i+3) {
       x.insert(i,QString(" "));
   }
    return x;
}
QString rem_spaces(QString str) {
    //convers
      str.remove(QChar(' '),Qt::CaseInsensitive);
      return str;
}
QString MainWindow::a(byte_t* bytes,int len) {
  return ins_spaces(b2QStringHex(bytes,len));
}
QString hexstr2Ascii(QString hexs){
    //converts an Ascii hexstring "30313233" in is equivalent AScii Ascii string "0123"
       QString Ascii;
       QByteArray ba = hexs.toLatin1();
       const char *c = ba.data();
       char z[5],n, *f;
       unsigned int i=0;
       for (i=0;i<=strlen(c);) {
            z[0] = '0';
            z[1] = 'x';
            z[2] = c[i];
            z[3] = c[i+1];
            z[4] = 0;
            n =strtol(z,&f,16);
            Ascii.append(n);
            i=i+2;
   }
       return Ascii;
}
void hexstr2byte(QString hexs,byte_t* b) {
    //VOID FUNCTION!!!declare byte array  b first!
    //converts an hex string "ffffffff" in is equivalent byte_t { '0xff','0xff','0xff','0xff' }
    QByteArray ba = hexs.toLatin1();
    const char *c = ba.data();
    char z[5],*f;
    unsigned int i=0;
    for (i=0;i<=strlen(c);i=i+2) {
        z[0]='0';
        z[1]='x';
        z[2]=c[i];
        z[3]=c[i+1];
        z[4]='\0';
        b[i/2]=strtol(z,&f,16);
    }
}
QString Ascii2hexstr(QString asciis) {
    //converts an ascii string in its hex equivalent
        QByteArray z;
        int f=asciis.size()-1;
        z.append(asciis);
        z.resize(f);
        return z.toHex().toUpper().data();

}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    iTimer = new QTimer(this);
    QObject::connect(iTimer, SIGNAL(timeout()), this, SLOT(TimeOut()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::update_status(const QString & text) {
    ui->textEdit_status->append(text);
    QScrollBar *sb = ui->textEdit_status->verticalScrollBar();
    sb->setValue(sb->maximum());
}
void MainWindow::keyPressEvent(QKeyEvent* e) {
   if (e->key()==Qt::Key_C) {
        Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
        bool isCtrl = keyMod.testFlag(Qt::ControlModifier);
       if (isCtrl) {
              stop=true;
        }
      }

}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

     default:
        break;
    }
}
void MainWindow::closeEvent(QCloseEvent *event) {
  }
void ByteToChar(byte_t * bytes, char* chars,int count){
     int i =0;
    for(; i < count; i++)
         chars[i] = (char)bytes[i];
}
void MainWindow::connect_reader() {
    pnd = nfc_connect(NULL); // Connect using the first available NFC device
    if (pnd != NULL) {
        initialize_device();
        connected=true; // Found device
    }
    else connected=false; // No device found
}
void MainWindow::initialize_device() {
    nfc_initiator_init(pnd); // Set connected NFC device to initiator mode
    nfc_configure(pnd,NDO_ACTIVATE_FIELD,false);     // Drop the field for a while
    nfc_configure(pnd,NDO_INFINITE_SELECT,false);   // Let the reader only try once to find a tag
    nfc_configure(pnd,NDO_HANDLE_CRC,true); // handle crc
    nfc_configure(pnd,NDO_HANDLE_PARITY,true); // handle parity
    nfc_configure(pnd,NDO_ACTIVATE_FIELD,true);   // Enable field so more power consuming cards can power themselves up
}
//create anticollision function
bool MainWindow::select_tag() {
   // initialize_device();
   if (nfc_initiator_select_tag(pnd,NM_ISO14443A_106,NULL,0,&nti)) {
     if (memcmp(last_target.nai.abtUid,nti.nai.abtUid,10)==0) {
         //same uid on the reader  we don't read again the tag
         tag_found=true;
         return false;
     }
     else {
        tag_found=true; // tag found
        ui->lineEdit_Uid->clear();
        ui->lineEdit_Card->clear();
        ui->tableWidget_Memory->clearContents();
        iTimer->stop();
        update_status("The Following NFC ISO14443A tag was found");
        update_status("ATQA (SENS_RES): "  + a(nti.nai.abtAtqa,2));
        update_status("UID (NFCID): "+ a(nti.nai.abtUid,nti.nai.szUidLen));
        memset(last_target.nai.abtUid,'\0',10);
        memcpy(last_target.nai.abtUid,nti.nai.abtUid,nti.nai.szUidLen);
        update_status("SAK (SEL_RES):" + a(&nti.nai.btSak,4));
        ui->lineEdit_Uid->setText(a(nti.nai.abtUid,nti.nai.szUidLen));
        get_card_type(nti.nai.abtAtqa,&nti.nai.btSak);
        update_status("Reading...");
        timerWait->singleShot(500,this,SLOT(timerupdate()));
        return true;
    }
 }
   else {
   if (!nfc_initiator_select_tag(pnd,NM_ISO14443A_106,last_target.nai.abtUid,last_target.nai.szUidLen,&nti))  {
       if (tag_found==true) {
           memset(last_target.nai.abtUid,'\0',10);
           memset(nti.nai.abtUid,'\0',10);
           update_status("Tag removed!");
//         clear();
           tag_found=false;
       return false;
      }
   }
}
   //   else { if (tag_found==false) { update_status("No tag in the field"); clear();  return false; }  }
}

void MainWindow::disconnect_reader() {
    //disconnects the reader usb device
      nfc_disconnect(pnd);	pnd = NULL; 	connected = false;
}
void MainWindow::get_card_type(byte_t* atqa,byte_t* sak) {
    //check bytes to identify tag type
    if ((atqa[1] == 0x04) && (sak[0] == 0x08)) { ui->lineEdit_Card->setText(" NXP - MIFARE Classic 1k");   card_type= 1; uiBlocks=0x3f; return; }
    if ((atqa[1] == 0x02) && (sak[0] == 0x18)) { ui->lineEdit_Card->setText(" NXP - MIFARE Classic 4k");   card_type= 2; uiBlocks=0xff;return; }
    if ((atqa[1] == 0x04) && (sak[0] == 0x09)) { ui->lineEdit_Card->setText(" NXP - MIFARE Classic Mini"); card_type= 3; uiBlocks=0x13;return; }
    if ((atqa[1] == 0x44) && (sak[0] == 0x00)) { ui->lineEdit_Card->setText(" NXP - MIFARE Ultralight");   card_type= 4; uiBlocks=0xf;return; }
    ui->lineEdit_Card->setText("Not Supported."); uiBlocks=0; card_type=0; return;

}
void MainWindow::on_pushButton_Connect_clicked()
{
    if (ui->pushButton_Connect->text()=="&Connect") {
//        if (connected==true) disconnect_reader();
        connect_reader();
        if (connected==false) {
         update_status("No Device Found..");
        }
        else {
             ui->pushButton_Connect->setText("&Disconnect");
             update_status("Connected to Device.Searching for tag");
             ui->lineEdit_Reader->setText(tr(pnd->acName));
             tag_found=false;
             timercounter=20;
             iTimer->start(500);
       }
    }
    else {
        ui->pushButton_Connect->setText("&Connect");
        ui->lineEdit_Reader->clear();
        clear();
        memset(last_target.nai.abtUid,'\0',10);
        disconnect_reader();
        connected=false;
        tag_found=false;
        iTimer->stop();
    }
}
void MainWindow::showEvent(QShowEvent *event)
 {
    if (first_show==false) {
     first_show=true;
     bUseKeyA=true;
     //reads keys from file, add them to listwidget
     QFile z("keys.txt");
     if (z.open(QIODevice::ReadOnly | QIODevice::Text)) {
         QTextStream in(&z);
       QString line = in.readLine();
       while (!line.isNull()) {
           ui->listWidget_keys->addItem(line.left(12).toLower());
           line=in.readLine();
       }
     }
     ui->textEdit_status->setReadOnly(true);
   //  ui->listWidget_keys->setSelectionMode(QAbstractItemView::SingleSelection);
     update_status("Welcome on Board");
     QStringList f;f.append("Sector");f.append("Block");f.append("Data Read");f.append("Write Ascii Data");f.append("Description");
     ui->tableWidget_Memory->setRowCount(255);
     ui->tableWidget_Memory->setColumnCount(5);
     ui->tableWidget_Memory->setHorizontalHeaderLabels(f);
     ui->tableWidget_Memory->setColumnWidth(0,40);
     ui->tableWidget_Memory->setColumnWidth(1,35);
     ui->tableWidget_Memory->setColumnWidth(2,250);
     ui->tableWidget_Memory->setColumnWidth(4,150);
   //  ui->tableWidget_Memory->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
     ui->radioButton_UseKeyA->setChecked(true);
     update_keys(); // save keys into byte array keys  used while authenticating to the tag
 }
}
void MainWindow::print_success_or_failure(bool bFailure, int block)
{
   if (bFailure) {
       QString str;
       str= QString("Can't read block %1").arg(block); // convert to dec format
       update_status(str);
   }
}
bool MainWindow::read_ultralight_card(nfc_device_t* pnd,unsigned int page_start,unsigned int page_end) {
  uint32_t page;
  bool bFailure = false;
  unsigned int i=page_start;
  for (page = page_start; page <= page_end;  page += 1)
  {
      // Skip this the first time, bFailure it means nothing (yet)
      if (bFailure) {
          //check if the same tag with the saved UID is still on the reader . anti collision
        if (!nfc_initiator_select_tag(pnd,NM_ISO14443A_106,nti.nai.abtUid,nti.nai.szUidLen,&nti))
          {
            update_status("Error while reading: tag removed.");
            memset(last_target.nai.abtUid,'\0',10);
            memset(nti.nai.abtUid,'\0',10);
            clear();
            tag_found=false;

            return false;
          }
          bFailure = false;

      }
      if (page != page_start)
      {
          print_success_or_failure(bFailure, page);
      }
    //  if (page == 2) {// develop parsing of lock bits in order to know single locked pages.lx..etc..
      // Try to read out page number page
      memset(mp.mpd.abtData,'\0',4);
      if (nfc_initiator_mifare_cmd(pnd,MC_READ,page,&mp))
      {
        ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(i),0));
        if (hex_mode==false) {
        ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(hexstr2Ascii(b2QStringHex(mp.mpd.abtData,4)),0));
    }
        else {
            ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(ins_spaces(b2QStringHex(mp.mpd.abtData,4)).toUpper(),0));
        }
        if (i==0)  ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem(QString("serial n."),0));
        if (i==1) ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem(QString("serial n."),0));
        if (i==2) ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem(QString("serial n./internal/lockbytes"),0));
        if (i==3) ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem(QString("OTP"),0));
        if (i>=4) ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem(QString("User Memory"),0));
      } else {
        bFailure = true;
      }
      i++;
  }
 if (!bFailure) {   update_status("Reading completed"); }

  return (!bFailure);
}
bool MainWindow::write_ultralight_card(nfc_device_t* pnd,byte_t* content,unsigned int page_start,unsigned int page_end,int len)
{
    unsigned int page;
    bool bFailure = false;
    bool try_again=false;
   for (page = page_start; page <= page_end; page++) {
       if (try_again) {
           if (!nfc_initiator_mifare_cmd(pnd, MC_WRITE,page, &mp)) {
               bFailure = true;
                 update_status(QString("Can't write page %1. Memory locked?").arg(page));
                 return false;
            }
           else {
               update_status(QString("Successfully wrote page %1").arg(page));
               try_again=false;
               continue;
         }
       }
             memset(mp.mpd.abtData, '\0',4); //bzero function empties data
             memcpy(mp.mpd.abtData,content,len);
             if (!nfc_initiator_mifare_cmd(pnd, MC_WRITE,page, &mp)) {
                 bFailure = true;
//                 update_status(QString("Can't write page %1").arg(page));
              }
             else {
                 update_status(QString("Successfully wrote page %1").arg(page));
                 continue;
             }
             if (bFailure) {
                if (nfc_initiator_select_tag(pnd,NM_ISO14443A_106,last_target.nai.abtUid,last_target.nai.szUidLen,&last_target)) {
                    bFailure=false;
                    page--;
                    try_again=true;
                }
                else { update_status("Error while writing: Tag  removed"); return false; }
            }
   }
}
bool is_trailer_block(uint32_t uiBlock)
{
  // Test if we are in the small or big sectors
  if (uiBlock < 128) return ((uiBlock+1)%4 == 0); else return ((uiBlock+1)%16 == 0);
}
uint32_t get_trailer_block(uint32_t uiFirstBlock)
{
  // Test if we are in the small or big sectors
  uint32_t trailer_block = 0;
  if (uiFirstBlock < 128) {
    trailer_block = uiFirstBlock + (3 - (uiFirstBlock % 4));
  } else {
    trailer_block = uiFirstBlock + (15 - (uiFirstBlock % 16));
  }
  return trailer_block;
}
uint32_t get_sector_number(uint32_t uiBlock) {
    uint32_t trailer_block = 0;
    if (uiBlock < 128) {
          trailer_block = (uiBlock + (3 - (uiBlock % 4)))/4;
    } else {
      trailer_block = (uiBlock)/16+24;
    }
    return trailer_block;
  }
bool MainWindow::authenticate(nfc_device_t* pnd,uint32_t uiBlock)
{
  mifare_cmd mc;
  int key_index;
  // Key file authentication.
  // Determin if we should use the a or the b key
   byte_t keya[6];
  mc = (bUseKeyA) ? MC_AUTH_A : MC_AUTH_B;
  //Set the authentication information (uid)
  memcpy(mp.mpa.abtUid,nti.nai.abtUid,4);
  if (!dont_loop) {
   memcpy(mp.mpa.abtKey,last_good_key,6);
  if (nfc_initiator_mifare_cmd(pnd, mc, uiBlock, &mp))
   {
     update_status(QString("Autheticated to sector %1 with last good key %2 mode: key%3").arg(get_sector_number(uiBlock)).arg(b2QStringHex(last_good_key,6)).arg(bUseKeyA?'A':'B'));
     return true;
   }
  // ABSOLUTELY NEEDED AFTER A FAILED AUTH!

  if (!nfc_initiator_select_tag(pnd, NM_ISO14443A_106, mp.mpa.abtUid, 4, NULL)) {
      return false;
  }
//    //loops through the selected keys only
   if (ui->checkBox_UseSelectedKeys->isChecked()) {
       QList<QListWidgetItem*> f=ui->listWidget_keys->selectedItems();
       unsigned int i=0;
       for (QList<QListWidgetItem *>::iterator it=f.begin();it!=f.end();it++)
         {
           hexstr2byte(f[i]->text(),keya);
           memcpy(mp.mpa.abtKey,keya,6);
           if (nfc_initiator_mifare_cmd(pnd, mc, uiBlock, &mp))
           {
             update_status(QString("Autheticated to sector %1 with key %2 mode: key%3").arg(get_sector_number(uiBlock)).arg(f[i]->text().toUpper()).arg(bUseKeyA?'A':'B'));
               /**
              * @note: what about the other key?
              */
             memcpy(last_good_key,keya,6); // copia la chiave buona!
             return true;
             break;
           }
           if (!nfc_initiator_select_tag(pnd, NM_ISO14443A_106, mp.mpa.abtUid, 4, NULL)) {
               return false;
           }
           i++;
       }
       return false;
   }
//loops through all the keys
  for (key_index = 0; key_index < ui->listWidget_keys->count(); key_index++)
    {
      memcpy(mp.mpa.abtKey, keys + (key_index*6), 6);
      if (nfc_initiator_mifare_cmd(pnd, mc, uiBlock, &mp))
      {
        update_status(QString("Autheticated to sector %1 with key %2 mode: key%3").arg(get_sector_number(uiBlock)).arg(ui->listWidget_keys->item(key_index)->text().toUpper()).arg(bUseKeyA?'A':'B'));
          /**
         * @note: what about the other key?
         */
        memcpy(last_good_key,keys+(key_index*6),6);

        return true;
        break;
      }
      if (!nfc_initiator_select_tag(pnd, NM_ISO14443A_106, mp.mpa.abtUid, 4, NULL)) {
          tag_found=false;
          return false;
      }
          // ABSOLUTELY NEEDED AFTER A FAILED AUTH!
    }
  if (ui->checkBox_ask->isChecked()==false) return false;
}
  //ASK for keys if checked
      bool ok=true;  QString hexkey;
      while (ok) {

      hexkey = QInputDialog::getText(this, QString("Authenticating sector %1").arg(get_sector_number(uiBlock)),
                                          QString("6Byte hex key (12digits) or 6Byte Ascii key (6digits) for sector %1").arg(get_sector_number(uiBlock)), QLineEdit::Normal,"FFFFFFFFFFFF"
                                          , &ok);
      QRegExp rx("[0-9a-fA-F]+"); // only hexadecimal digits allowed
      if (ok) {
          if ((hexkey.length()==12)) {
             if (rx.exactMatch(hexkey)) {
               break;
             }
             else { update_status("Invalid Hex Key format"); continue; }
         }
         if ((hexkey.length()==6)) {
              hexkey.append(QChar('\0'));
              hexkey=Ascii2hexstr(hexkey);
              update_status("Ascii Authentication");
              break;
             }
       else { update_status("Invalid key length: 12 hex digits or 6 ascii digits"); }

        }
      else { return false; }
  }
      QMessageBox msgBox;
      msgBox.setWindowTitle("Mifare Diagnostic Tool");
      msgBox.setWindowIcon(this->windowIcon());
      msgBox.setInformativeText("Press Yes for KeyA - No for KeyB");
      msgBox.setText(QString("Use Key A?"));
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Yes);
      msgBox.setIcon(QMessageBox::Question);
      int ans=msgBox.exec();
      switch (ans) {
      case QMessageBox::Yes:
             mc = MC_AUTH_A;
          break;
      case QMessageBox::No:
             mc = MC_AUTH_B;
      break;
      case QMessageBox::Cancel:
               return false;
               break;
      default:
      return false;
      break;
      }
      byte_t inputkey[6];
      hexstr2byte(hexkey,inputkey);
      memcpy(mp.mpa.abtKey ,inputkey,6);
      if (nfc_initiator_mifare_cmd(pnd, mc, uiBlock, &mp))
       {
          update_status(QString("Autheticated to sector %1 with key %2 mode: Key%3").arg(get_sector_number(uiBlock)).arg(hexkey).arg((ans==QMessageBox::Yes)?'A':'B'));
          return true;
       }

//       nfc_initiator_select_tag(pnd, NM_ISO14443A_106, nti.nai.abtUid, 4,&nti);
 return false;
}

bool MainWindow::read_classic_card2(nfc_device_t* pnd,int sectorblock,int blockstop) {
    int32_t iBlock;
    bool bFailure = false;
    int i,g; // i is used to get the row we should edit in the QTableview
    i = (sectorblock < 3) ? 0: (sectorblock <128) ? blockstop-3 : blockstop - 15;

    memcpy(mp.mpa.abtUid,nti.nai.abtUid,4); //copies UID info
    for (iBlock=sectorblock; iBlock<=blockstop; iBlock++) {

        if (!is_trailer_block(iBlock) ) {
            continue;
    }
    else {

       if (iBlock < 128) g=3; else g=15; // to distinguish between mifare 1k and mifare 4k
           // Try to authenticate for the current sector
       if (bFailure)         {
        if (!nfc_initiator_select_tag(pnd,NM_ISO14443A_106,mp.mpa.abtUid, 4,&nti))
         {
          update_status("Error while reading: tag removed."); //try  catch ? debug

          memset(last_target.nai.abtUid,'\0',4);
          memset(nti.nai.abtUid,'\0',10);
          tag_found=false;
          clear();
          return false;
         }
        bFailure = false;
      }
       if (!authenticate(pnd,iBlock))
       {
           ui->tableWidget_Memory->setItem(i,0,new QTableWidgetItem(QString("%1").arg(get_sector_number(iBlock)),0));
           ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g),0));
           ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem("Authentication Failed ",0));
           update_status(QString("Authentication failed to sector %1").arg(get_sector_number(iBlock)));
           bFailure=true;
           i++;
           continue;
       }
       int32_t block2=iBlock;
       ui->tableWidget_Memory->setItem(i,0,new QTableWidgetItem(QString("%1").arg(get_sector_number(iBlock)),0));
       for (;g>0;g--) block2--;
       for (;block2<=iBlock;block2++) {
        // Try to read out the trailer
           if (bFailure) {
               if (!nfc_initiator_select_tag(pnd,NM_ISO14443A_106,mp.mpa.abtUid, 4,&nti))
                {
                 update_status("Error while reading: tag removed."); //try  catch ? debug
                 memset(last_target.nai.abtUid,'\0',10);
                 memset(nti.nai.abtUid,'\0',10);
                 tag_found=false;
              //   clear();
                 return false;
                }
               bFailure = false;
             }

         ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g),0));g++;
         if (nfc_initiator_mifare_cmd(pnd,MC_READ,block2,&mp))  {
           if (hex_mode) {
                 ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(ins_spaces(b2QStringHex(mp.mpd.abtData,16)),0));
             }
             else {
                 ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(hexstr2Ascii(b2QStringHex(mp.mpd.abtData,16)),0));
             }
             if (is_trailer_block(block2)) {
                 ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem("Trailer(KeysA.AccessBits.KeysB)",0));
                 ui->tableWidget_Memory->item(i,2)->setToolTip("Double click to parse trailer access bits");
             }
             else {
                 if (block2==0) {   ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem("Manufacturer Block",0)); }
                 else {  ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem("User Memory",0)); }
             }
          }
       else {
           ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(QString("Cant Read Block ")));
           bFailure=true;
       }

       i++;
     }
   }

}
    return true;
}


bool MainWindow::read_classic_card(nfc_device_t* pnd,int sectorblock,int block_stop)
{
  int32_t iBlock;
  bool bFailure = false;
  int i=uiBlocks-sectorblock,g=0; // i is used to get the row we should edit in the QTableview
  memcpy(mp.mpa.abtUid,nti.nai.abtUid,nti.nai.szUidLen); //copies UID info
  for (iBlock=sectorblock; iBlock>=block_stop; iBlock--)
  {
      if (bFailure)
      {
         nfc_initiator_deselect_tag(pnd);
         if (!nfc_initiator_select_tag(pnd,NM_ISO14443A_106,mp.mpa.abtUid,4,&nti)) {
             update_status("Error while reading: tag removed!"); //try debug
             memset(last_target.nai.abtUid,'\0',10);
             memset(nti.nai.abtUid,'\0',10);
             tag_found=false;
             return false;
           }
         else {
         bFailure=false;
      }
     }
    // Authenticate everytime we reach a trailer block
    if (is_trailer_block(iBlock))
    {
      if (iBlock < 128) g=3; else g=15; // to distinguish between mifare 1k and mifare 4k
          // Try to authenticate for the current sector
      if (!authenticate(pnd,iBlock))
      {
          ui->tableWidget_Memory->setItem(i,0,new QTableWidgetItem(QString("%1").arg(get_sector_number(iBlock)),0));
          ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g),0));
          ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem("Authentication Failed ",0));
          update_status(QString("Authentication failed to sector %1").arg(get_sector_number(iBlock)));
          iBlock=iBlock-g;
          for (;g>=0;g--) {
            i++;
            ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(QString("Cant Read Block "),0));
            ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g-1),0));
          }
          bFailure=true;
          continue;
      }
      // ? save last_good_key key in a separate txt file?

      // Try to read out the trailer
      ui->tableWidget_Memory->setItem(i,0,new QTableWidgetItem(QString("%1").arg(get_sector_number(iBlock)),0));
      ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g),0));
      if (nfc_initiator_mifare_cmd(pnd,MC_READ,iBlock,&mp))
      {
            if (hex_mode) {
                ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(ins_spaces(b2QStringHex(mp.mpd.abtData,16)),0));
            }
            else {
                ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(hexstr2Ascii(b2QStringHex(mp.mpd.abtData,16)),0));
            }
            ui->tableWidget_Memory->item(i,2)->setToolTip("Double click to parse trailer access bits");
         //   ui->tableWidget_Memory->item(i,3)->setToolTip("Double click to write sector trailer block");
            ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem("Trailer(KeysA.AccessBits.KeysB)",0));
            g--;

      }
      else {
          ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(QString("Cant Read Block "),0));
          g--;
          bFailure=true;
          }
    } else {
      //Make sure a earlier readout did not fail
      if (!bFailure)
      {
        // Try to read out the data block
        if (nfc_initiator_mifare_cmd(pnd,MC_READ,iBlock,&mp))
        {
              ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g),0));
              if (hex_mode) {
                ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(ins_spaces(b2QStringHex(mp.mpd.abtData,16)),0));
              }
              else {
                ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(hexstr2Ascii(b2QStringHex(mp.mpd.abtData,16)),0));
              }
          if (iBlock!=0)        ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem("User Memory"));
          else ui->tableWidget_Memory->setItem(i,4,new QTableWidgetItem("Manufacturer block"));
          g--;
      //  ui->tableWidget_Memory->item(i,3)->setToolTip("Start typing to write block");

        } else {
          ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g),0));
          ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(QString("Cant Read Block ")));
          g--;
        }
      }
      else {
          ui->tableWidget_Memory->setItem(i,1,new QTableWidgetItem(QString("%1").arg(g),0));
          ui->tableWidget_Memory->setItem(i,2,new QTableWidgetItem(QString("Cant Read Block "),0));
          g--;
          bFailure=true;
          }
    }
    i++;
}
  update_status("Reading Completed");
 return true;

}

void MainWindow::read() {
    if (connected==true) {
        ui->tableWidget_Memory->scrollToTop();
        ui->tableWidget_Memory->clearContents();
        ui->tableWidget_Memory->setUpdatesEnabled(true);
        this->setDisabled(true);
    if (card_type != 4)
    {
        if (ui->listWidget_keys->count()==0) { update_status("No Keys Inserted!. Add at least one key"); return; }
        ui->tableWidget_Memory->setHorizontalHeaderItem(1,new QTableWidgetItem("Block",0));
        ui->tableWidget_Memory->showColumn(0);
        //  read_classic_card2(pnd,0,uiBlocks);
        read_classic_card(pnd,uiBlocks,0);
}
    else {
        ui->tableWidget_Memory->setHorizontalHeaderItem(1,new QTableWidgetItem("Page",0));
        ui->tableWidget_Memory->hideColumn(0);
        read_ultralight_card(pnd,0,15);
    }
    ui->tableWidget_Memory->setUpdatesEnabled(true);
    this->setDisabled(false);
    timercounter=20;
    initialize_device();
    iTimer->start(500);
 }

}
void CharToByte(char* chars, byte_t* bytes, int count){
     int i=0;
    for(; i < count; i++)         bytes[i] = (byte_t )chars[i];
}
bool MainWindow::write_classic_card(nfc_device_t* pnd,int block,byte_t* content,int len)
{
  bool bFailure = false;
  int sector=get_sector_number(block);
  int32_t sector_trailer=get_trailer_block(block);
  if (is_trailer_block(block)) {

      if (!authenticate(pnd,sector_trailer)) {
          update_status(QString("Cannot Authenticate to sector %1").arg(sector));
          return false;
      }
       memcpy(mp.mpd.abtData,content,16);
      // Try to write the trailer
          if (nfc_initiator_mifare_cmd(pnd,MC_WRITE,block,&mp) == false) {
        update_status(QString("Failed to write sector trailer %1").arg(sector));
        return false;
     }
      update_status(QString("Successfully wrote sector trailer %1 with data %2").arg(sector).arg(b2QStringHex(content,16)));   return false;
  }
  else {
        if (!authenticate(pnd,sector_trailer)) {
            update_status(QString("Cannot Authenticate to sector %1").arg(sector));
            return false;
        }
        //check for anticollision!?
        if (block == 0) { return false; update_status("Block 0 is read only"); } // first block is read-only;
        memset(mp.mpd.abtData, '\0',16); //bzero function empties the data
        memcpy(mp.mpd.abtData,content,len); //copies content to mp
        if (!nfc_initiator_mifare_cmd(pnd,MC_WRITE,block,&mp)) bFailure = true;
        if (bFailure==true) { update_status("Cannot Write Block"); return false; }
        else { update_status(QString("Successfully written sector %1 block %2").arg(sector).arg((block<128)?block%4:block%16)); }
        return true;
  }
}
void MainWindow::on_tableWidget_Memory_cellClicked(int row, int column)
{
     ui->tableWidget_Memory->setEditTriggers(QAbstractItemView::NoEditTriggers);
     //switch to hex/ascii when clicked
//     if (column==2) {
//
//        if (ui->tableWidget_Memory->item(row,2) != 0)  {
//            QString str ;
//            str=ui->tableWidget_Memory->item(row,2)->text();
//        if (ui->tableWidget_Memory->item(row,2)->data(32) == 1) {
//             ui->tableWidget_Memory->setItem(row,2,new QTableWidgetItem(hexstr2Ascii(rem_spaces(str)),0));
//             ui->tableWidget_Memory->item(row,2)->setData(32,0);
//        }
//        else {
//              ui->tableWidget_Memory->setItem(row,2,new QTableWidgetItem(ins_spaces(Ascii2hexstr(str)),0));
//              ui->tableWidget_Memory->item(row,2)->setData(32,1);
//        }
//      }
//    }
     if (column==3) {
         if (((card_type==4) && (row>=4)) || (card_type !=4)) {
             ui->tableWidget_Memory->setEditTriggers(QAbstractItemView::AnyKeyPressed);
    }
     }
 }
bool MainWindow::valids(QString s) {
    QRegExp rx("[0-9a-fA-F]+");
    if (hex_mode==true) {
        if (!rx.exactMatch(s)) { update_status("Only hex digits allowed format: 01234abcef");  return false; }
        if (s.length() %2!=0) {  update_status("Invalid hex string: You must input an even number of hex digits  ");   return false;  }
        if (((card_type==4) && (s.length()> 8)) || ((card_type!=4) && (s.length() >32))) { update_status("Hex string too long");  return false;  }

    }
    else {
        if (card_type==4 && (s.length()>4)) { update_status("Cannot input more than 4 Ascii Characters");  return false;  }
        if (card_type!=4 && (s.length()>16)) { update_status("Cannot input more than 16 Ascii Characters");   return false;  }
    }
 return true;
}

void MainWindow::on_tableWidget_Memory_cellChanged(int row, int column)
{
    if (column==3) {
        QString s;
        s=ui->tableWidget_Memory->currentItem()->text();
        if (valids(s)) {
         int block=uiBlocks-row;
         uint32_t z=get_trailer_block(block);
         if ((card_type !=4) && (is_trailer_block(block))) {
            return;
        }
        iTimer->stop();
        QMessageBox msgBox;
        msgBox.setWindowTitle("Mifare Diagnostic Tool");
        msgBox.setWindowIcon(this->windowIcon());
        if (card_type==4) {
        msgBox.setText(QString("You are going to write %1 in page %2").arg(s).arg(row));
    }
        else {
             msgBox.setText(QString("You are going to write %1 in sector %2 block %3").arg(s).arg(get_sector_number(block)).arg(block<128?block%4:block%16));
        }
        msgBox.setInformativeText("Proceed?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Question);
        int ret=msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
        //write column!
        if (hex_mode ==false) {
        if (card_type==4) {
         // write ultralight in ascii mode
           if (row>=4) {
             if (s.isEmpty()  ==false) {
            write_ultralight_card(pnd,QStr2Byte(s),row,row,s.length());
         }
           else {
                byte_t n[]={0x00,0x00,0x00,0x00};
                write_ultralight_card(pnd,n,row,row,4);
          }
          read_ultralight_card(pnd,row,row);
        }
       }
   else {
         if (block!=0) {
            write_classic_card(pnd,block,QStr2Byte(s),s.length());
            read_classic_card(pnd,z,block);
      //      read_classic_card2(pnd,block,z);
      }
          else {
              update_status("Block 0 is read only");
          }
      }
    }
        else {
            
            byte_t b[16];
            memset(b, '\0',16);
            hexstr2byte(s,b);
            if (card_type==4) {
               // write ultralight in hex mode
                 if (row>=4) {
                  write_ultralight_card(pnd,b,row,row,4);
                             }
                 read_ultralight_card(pnd,row,row);
              }
            else {
                if (block!=0) {
                  write_classic_card(pnd,block,b,16);
                  read_classic_card(pnd,z,block);
            }
                else {
                    update_status("Block 0 is read only");
                }

            }
        }

      break;
      case QMessageBox::No:
                      return ;
      break;
      case QMessageBox::Cancel:
               return ;
      default:
      return;
      break;
    }
  }
 iTimer->start(500);
}
}
void MainWindow::on_checkBox_clicked()
{
    if (ui->checkBox->isChecked())  {
        hex_mode=true;
        ui->tableWidget_Memory->horizontalHeaderItem(3)->setText("Write HEX Data");
        unsigned int row=0;
        for (;row<=uiBlocks;row++) {
        if (ui->tableWidget_Memory->item(row,2)==0) { continue; return; }
        QString str;
        str=ui->tableWidget_Memory->item(row,2)->text();
        if ((str=="Authentication Failed ") || (str=="Cant Read Block ")) { continue; }

         if (ui->tableWidget_Memory->item(row,2)->data(32) != 1) {
           ui->tableWidget_Memory->setItem(row,2,new QTableWidgetItem(ins_spaces(Ascii2hexstr(str)),0));
           ui->tableWidget_Memory->item(row,2)->setData(32,1); //32 UserSpecific Role
        }
       }

    }
     else {
         hex_mode=false;
         ui->tableWidget_Memory->horizontalHeaderItem(3)->setText("Write Ascii Data");

         unsigned int row=0;
         for (;row<=uiBlocks;row++) {
         if (ui->tableWidget_Memory->item(row,2)==0) { continue; return; }
         QString str;
         str=ui->tableWidget_Memory->item(row,2)->text();
         if ((str=="Authentication Failed ") || (str=="Cant Read Block ")) { continue; }
         if (ui->tableWidget_Memory->item(row,2)->data(32) != 0) {
          ui->tableWidget_Memory->setItem(row,2,new QTableWidgetItem(hexstr2Ascii(rem_spaces(str)),0));
          ui->tableWidget_Memory->item(row,2)->setData(32,0);
      }
     }
    }
  }
void MainWindow::on_radioButton_UseKeyA_clicked()
{
    bUseKeyA=true;
    update_status("Using keyA");
}
void MainWindow::on_radioButton_UseKeyB_clicked()
{
    bUseKeyA=false;
    update_status("Using keyB");

}
void MainWindow::on_pushButton_AddKey_clicked()
{
    //further associate key mode to key...
     QString z;
     z=ui->lineEdit_AddKey->text();
     if (valid_key(z)) { add_key(z); }
 }
void MainWindow::update_key_file() {
    QFile::remove("keys.txt");
    QFile file("keys.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text))
         return;
    int i=0;
    for (i=0;i<ui->listWidget_keys->count();i++) {
    QTextStream keys(&file);
     keys << ui->listWidget_keys->item(i)->text() << "\n";
    }
    file.close();
}

void MainWindow::on_pushButton_RemKey_clicked()
{
    qDeleteAll(ui->listWidget_keys->selectedItems());
    update_key_file();
    update_keys();
   }

void MainWindow::update_keys() {
    memset(keys,'\0',360);
    for(int i=0;i<ui->listWidget_keys->count();i++) {
       hexstr2byte(ui->listWidget_keys->item(i)->text(),keys + (i*6));
   }
}
void MainWindow::clear() {
 //   ui->lineEdit_Uid->clear();
 //   ui->lineEdit_Card->clear();
   // ui->lineEdit_Reader->clear();
 //   ui->tableWidget_Memory->clearContents();;
//    ui->textEdit_status->clear();
}


void MainWindow::on_pushButton_clicked()
{
   if (pnd!=NULL) disconnect_reader();
    QCoreApplication::exit(0);
}
void MainWindow::on_listWidget_keys_itemClicked(QListWidgetItem* item)
{
    ui->lineEdit_AddKey->setText(ui->listWidget_keys->currentItem()->text());
}
void MainWindow::parse_trailer(byte_t* data) {
        byte_t AC[4]; //access condition
        byte_t mask = 0x01;
        int i=0;
        memcpy(AC, data+6, 4); //copies the access bits bytes from 6to10
//
        update_status(QString("Access Conditions: %1").arg(b2QStringHex(AC,4)));
        update_status( "AC matrix (x = bit set to 1; - = bit set to 0)\n");
        update_status("  C1 C2 C3 " );
        for (i = 0; i <= 3; i++)
                acs[i].c1 = ((AC[1] & (mask << (i+4))) > 0);
        for (i = 0; i <= 3; i++)
                acs[i].c2 = ((AC[2] & (mask << (i))) > 0);
        for (i = 0; i <= 3; i++)
                acs[i].c3 = ((AC[2] & (mask << (i+4))) > 0);
        for (int i=3; i >= 0; i--) {
                update_status(QString("| %1 %2 %3 | block %4 %5").arg((acs[i].c1 ?" x " : " - ")).arg((acs[i].c2 ? " x " : " - ")).arg((acs[i].c3 ? " x " : " - ")).arg(i).arg((i == 3) ? " (trailer) " : ""));
        }
                update_status("-- Trailer block has following AC set: ");
        if ((!acs[3].c1) && (!acs[3].c2) && (!acs[3].c3))
                update_status("Key A may be written by itself, AC may be read by key A, key B may be read and written by key A.");
        if ((!acs[3].c1) && (acs[3].c2) && (!acs[3].c3))
                update_status( "Key A cannot be changed, AC may be read by key A, key B may be read by key A.");
        if ((acs[3].c1) && (!acs[3].c2) && (!acs[3].c3))
                update_status( "Key A may be changed by key B, AC may be read by both keys, key B may be changed by key B.");
        if ((acs[3].c1) && (acs[3].c2) && (!acs[3].c3))
                update_status( "AC may be read by both keys, nothing else on the trailer block allowed.");
        if ((!acs[3].c1) && (!acs[3].c2) && (acs[3].c3))
                update_status( "Transport configuration, everything on trailer block (except reading key A) may be done by key A.");
        if ((!acs[3].c1) && (acs[3].c2) && (acs[3].c3))
                update_status( "Key A may be changed by key B, AC may be read by both keys, changed by key B, key B may be changed by key B.");
        if ((acs[3].c1) && (!acs[3].c2) && (acs[3].c3))
                update_status(  "AC may be read by both keys, written by key B. Nothing else on the trailer block allowed.");
        if ((acs[3].c1) && (acs[3].c2) && (acs[3].c3))
                update_status(  "AC may be read by both keys, nothing else on the trailer block allowed.");
        for (int i=2; i >= 0; i--) {
                update_status(QString("-- Block %1  (relative to sector beginning) has following AC set:").arg(i));
                if ((!acs[i].c1) && (!acs[i].c2) && (!acs[i].c3))
                        update_status("Transport configuration; may be read, written, incremented and decremented by both keys.");
                if ((!acs[i].c1) && (acs[i].c2) && (!acs[i].c3))
                       update_status("May be read by both keys, read-only.");
                if ((acs[i].c1) && (!acs[i].c2) && (!acs[i].c3))
                        update_status("May be read by both keys, written by key B. Value operations not allowed.");
                if ((acs[i].c1) && (acs[i].c2) && (!acs[i].c3))
                        update_status("May be read and decremented by both keys, written and incremented only by key B");
                if ((!acs[i].c1) && (!acs[i].c2) && (acs[i].c3))
                        update_status("Decrement-only block. May be read and decremented by both keys. Nothing else allowed.");
                if ((!acs[i].c1) && (acs[i].c2) && (acs[i].c3))
                        update_status("May be accessed (read/write) only by key B. Value operations not allowed.");
                if ((acs[i].c1) && (!acs[i].c2) && (acs[i].c3))
                        update_status("May be read only by key B, read-only.");
                if ((acs[i].c1) && (acs[i].c2) && (acs[i].c3))
                        update_status("Dead block, no operation is allowed.");
        }
}
bool MainWindow::valid_key(QString str) {
    QRegExp rx("[0-9a-fA-F]+"); // only hexadecimal digits allowed
    if ((str.length()==12)) {
           if (rx.exactMatch(str)) {
             return true;
           }
           else { update_status("Invalid Hex Key format"); return false; }
    }
    if ((str.length()==6)) {
        return true;
   }
    else { update_status("Invalid key length: 12 hex digits or 6 ascii digits"); return false; }
}

void MainWindow::on_tableWidget_Memory_cellDoubleClicked(int row, int column)
{
    int block=uiBlocks-row;

    if (column==2) {
    if (card_type!=4) {
        if (is_trailer_block(block)) {
            if (ui->tableWidget_Memory->item(row,2)!=0) {
                 QString s;
                 s=ui->tableWidget_Memory->item(row,2)->text();
                 if (s=="Authentication Failed " || s.isEmpty() || s=="Cant Read Block ") return;
                 byte_t b[16];
                 memset(b, '\0',16);

                if (hex_mode==true) {
                    hexstr2byte(rem_spaces(s),b);
                    parse_trailer(b);
                }
                else {
                    hexstr2byte(Ascii2hexstr(s),b);
                    parse_trailer(b);
                }

            }
        }
      }
    }
    if ((card_type!=4) && (column==3) && (is_trailer_block(block))) {
       bool ok;
       QString KeyA,AccB,KeyB,data;
            KeyA = QInputDialog::getText(this, tr("Writing Sector Trailer"),
                                                tr("6Byte hex key (12digits) or 6Byte Ascii key (6digits) for KeyA"), QLineEdit::Normal,""
                                                , &ok);
           QRegExp rx("[0-9a-fA-F]+");
             if (ok==true) {
                 if (valid_key(KeyA)) {

               AccB = QInputDialog::getText(this, tr("Writing Sector Trailer"),
                                         tr("4Byte hex AccessBits(8digits) (Default: Transport configuration)"), QLineEdit::Normal,"FF078069"
                                           , &ok);
           }  else { update_status("Wrong key format"); return; }
         }  else { return; }
            if (ok==true) {
          if (rx.exactMatch(AccB) && AccB.length()==8) {

             KeyB = QInputDialog::getText(this, tr("Writing Sector Trailer"),
                                                tr("6Byte hex key (12digits) or 6Byte Ascii key (6digits)"), QLineEdit::Normal,""
                                               , &ok);
           } else { update_status("Wrong key format"); return; }
       } else return;
           if (ok==true) {
            if (valid_key(KeyB)) {
                    if (KeyA.length()==6) { KeyA.append(QChar('\0')); KeyA=Ascii2hexstr(KeyA); }
                    if (KeyB.length()==6) { KeyB.append(QChar('\0')); KeyB=Ascii2hexstr(KeyB); }
                    data = KeyA+AccB+KeyB;
                  }
                 else { update_status("Wrong key format"); return; }
            }
            else return;

            QMessageBox msgBox;
            msgBox.setWindowTitle("Mifare Diagnostic Tool");
            msgBox.setWindowIcon(this->windowIcon());
            msgBox.setInformativeText(QString("Remember to save your new keys!\nProceed writing sector trailer %1?").arg(get_sector_number(block)));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setDetailedText(tr("You are going to write the following data to the sector trailer:%1\n").arg(get_sector_number(block)) + data + tr("\nKeyA: ") + KeyA + tr("\nAccessBits: ") + AccB + tr("\nKeyB: ")+ KeyB );
            int ans=msgBox.exec();
            switch (ans) {
            case QMessageBox::Yes:
                    byte_t n[16];
                    memset(n,'\0',16);
                    hexstr2byte(data,n);
                    add_key(KeyA);
                    add_key(KeyB);
                    write_classic_card(pnd,block,n,16);
                   read_classic_card(pnd,block,block-1);
//                    read_classic_card2(pnd,row,get_trailer_block(row));

                break;
            case QMessageBox::No:
                return ;
            break;
            case QMessageBox::Cancel:
                     return ;
            default:
            return;
            break;
            }
      }
}
void MainWindow::parse_lockbytes(byte_t* data) {
// develop parsing of ultralight lock bytes!
// develop parsing of access bits
}
void MainWindow::TimeOut(void) {
    select_tag();
//    timercounter=timercounter-1;
//    if (timercounter==0) {
//      if (tag_found==false)  update_status("Tag not Found");
//        iTimer->stop();
//    }
}
void MainWindow::timerupdate() {
    if (wait==0) read();
}


void MainWindow::on_checkBox_loop_clicked()
{
    if (ui->checkBox_loop->isChecked()) {
            dont_loop =true;
    }
    else
        dont_loop = false;
}
void MainWindow::moveKeyDown() {
    QListWidgetItem *current = ui->listWidget_keys->currentItem();
    int currIndex = ui->listWidget_keys->row(current);

    QListWidgetItem *next = ui->listWidget_keys->item(ui->listWidget_keys->row(current) + 1);
    int nextIndex = ui->listWidget_keys->row(next);

    QListWidgetItem *temp = ui->listWidget_keys->takeItem(nextIndex);
    ui->listWidget_keys->insertItem(currIndex, temp);
    ui->listWidget_keys->insertItem(nextIndex, current);
}

void MainWindow::moveKeyUp() {
    QListWidgetItem *current = ui->listWidget_keys->currentItem();
    int currIndex = ui->listWidget_keys->row(current);

    QListWidgetItem *prev = ui->listWidget_keys->item(ui->listWidget_keys->row(current) - 1);
    int prevIndex = ui->listWidget_keys->row(prev);

    QListWidgetItem *temp = ui->listWidget_keys->takeItem(prevIndex);
    ui->listWidget_keys->insertItem(prevIndex, current);
    ui->listWidget_keys->insertItem(currIndex, temp);
}
void MainWindow::on_pushButton_moveUp_clicked()
{
    moveKeyUp();
    update_key_file();
    update_keys();
}

void MainWindow::on_pushButton_moveDown_clicked()
{
    moveKeyDown();
    update_key_file();
    update_keys();
}
void MainWindow::add_key(QString key) {
    if (key.length()==6) { key.append(QChar('\0')); key=Ascii2hexstr(key); }
      if (ui->listWidget_keys->count()!=60) {
       QList<QListWidgetItem *> f;
          f=ui->listWidget_keys->findItems(key.toLower(),Qt::MatchExactly);
           if (f.isEmpty()) {
               QFile file("keys.txt");
               if (!file.open(QIODevice::Append | QIODevice::Text))
                 return;
               QTextStream keys(&file);
               keys << key << "\n";
               file.close();
               ui->listWidget_keys->insertItem(0,key.toLower());
               update_status(QString("Hex:%1         Ascii:%2       added").arg(key).arg(hexstr2Ascii(key)));
               update_keys();
}
    else {  update_status("Key already in list"); }
}
else { update_status("Maximum Keys Number Reached"); }
}

void MainWindow::on_pushButton_ClearMemory_clicked()
{
    if ((connected==true) && (tag_found==true) && (card_type!=4)) {
    iTimer->stop();
    QMessageBox msgBox;
    msgBox.setWindowTitle("Mifare Diagnostic Tool");
    msgBox.setWindowIcon(this->windowIcon());
    msgBox.setText(QString("Are you sure you want to RESET your tag to factory Values?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Warning);
    int ans=msgBox.exec();
    switch (ans) {
    case QMessageBox::Yes:
        byte_t d[16];
        memset(d,'\0',16);

        if (card_type==4) {
            write_ultralight_card(pnd,d,4,15,4);
            read_ultralight_card(pnd,0,15);
        }
        else {
            int i=uiBlocks;
            for (;i>=0;i--) {

                   if (!is_trailer_block(i)) write_classic_card(pnd,i,d,16);

            }
            for (i=uiBlocks;i>=0;i--) {
                    if (is_trailer_block(i)) {
                        byte_t k[]={ 0x00,0x00,0x00,0x00,0x00,0x00, // def. keya
                                     0xFF,0x07,0x80,0x69,           // transport conf
                                     0xff,0xff,0xff,0xff,0xff,0xff, // def keyb
                                 };
                       write_classic_card(pnd,i,k,16);
                    }
            }
             read_classic_card(pnd,uiBlocks,0);
        }
        break;
    case QMessageBox::No:
        iTimer->start();
        return;
    break;
    case QMessageBox::Cancel:
    iTimer->start();

             break;
    default:
    break;

}
iTimer->start();
}
}


void MainWindow::on_checkBox_UseSelectedKeys_clicked()
{
    QList<QListWidgetItem*> f=ui->listWidget_keys->selectedItems();
    unsigned int i=0;
    for (QList<QListWidgetItem *>::iterator it=f.begin();it!=f.end();it++)
      {
//    update_status(f[i]->text());
    i++;
}

}
//void MainWindow::on_tableWidget_Memory_keyPressEvent(QKeyEvent* e) {
//    if (e->key()==Qt::Key_Delete) {
//        update_status("UES");
//}
//}


void MainWindow::on_pushButton_RefreshMem_clicked()
{
    if ((connected==true) && (tag_found==true)) {
    update_status("Refreshing Memory");
    read();
}
}

// DUMP FILE

typedef struct {
        byte_t KeyA[6];
        byte_t KeyB[6];
        bool foundKeyA;
        bool foundKeyB;
        byte_t trailer;                         // Value of a trailer block
} sector;
typedef struct {
        nfc_target_info_t ti;
        sector *        sectors;                // Allocate
        uint32_t        num_sectors;
        uint32_t        num_blocks;
        uint32_t        uid;
        bool            b4K;
} mftag;
void mf_configure(nfc_device_t* pdi) {
        nfc_initiator_init(pdi);
        // Drop the field for a while, so can be reset
        nfc_configure(pdi,NDO_ACTIVATE_FIELD,false);
        // Let the reader only try once to find a tag
        nfc_configure(pdi,NDO_INFINITE_SELECT,false);
        // Configure the CRC and Parity settings
        nfc_configure(pdi,NDO_HANDLE_CRC,true);
        nfc_configure(pdi,NDO_HANDLE_PARITY,true);
        // Enable the field so more power consuming cards can power themselves up
        nfc_configure(pdi,NDO_ACTIVATE_FIELD,true);
}
void mf_anticollision(mftag t,nfc_device_t* pdi) {
        if (!nfc_initiator_select_tag(pdi, NM_ISO14443A_106, NULL, 0, &t.ti)) {
                fprintf(stderr, "\n\n!Error: tag has been removed\n");
                exit(1);
        }
}
void mf_select_tag(nfc_device_t* pdi,nfc_target_info_t* ti) {
        // Poll for a ISO14443A (MIFARE) tag
        if (!nfc_initiator_select_tag(pdi,NM_ISO14443A_106,NULL,0,ti)) {
                fprintf(stderr, "!Error connecting to the MIFARE Classic tag\n");
                nfc_disconnect(pdi);
                exit(1);
        }
}
void MainWindow::on_pushButton_DumpMemory_clicked()
{
    if ((connected==true) && (tag_found==true) && (card_type !=4)) {
        update_status("Trying to Dump memory");
          iTimer->stop();
          mftag		t;
          int i,block,key_index,succeed,failure;
          byte_t last_keyA[6];
          byte_t last_keyB[6];
          mifare_cmd mca;
          mifare_cmd mcb;
          mca = MC_AUTH_A;
          mcb = MC_AUTH_B;
          memcpy(mp.mpa.abtUid,nti.nai.abtUid,4);
          //1k  0x3f           //4k 0xff            //mini 0x13
          t.num_blocks = (card_type == 1) ? 0x3f : (card_type==2) ? 0xff : (card_type==3) ? 0x13 : 0 ;
          t.num_sectors = (card_type == 1) ? 16 : (card_type==2) ? 40 : (card_type==3) ? 5 : 0;
          t.sectors = (sector *) calloc(t.num_sectors, sizeof(sector));
          QString s=a(nti.nai.abtUid,nti.nai.szUidLen);
          s.append(".dump");
          QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                          s,
                                      tr("Dump File (*.dump)"));

          if (!fileName.isNull()) {
// ADD CHECKS ON FILE!! can open?
         if (t.sectors == NULL) {
              update_status("Cannot allocate memory for t.sectors\n");
         }
         for (i = 0; i < (t.num_sectors); ++i) {
                 t.sectors[i].foundKeyA = t.sectors[i].foundKeyB = false;
         }

         i = 0; // Sector counter
         nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
         for (block = 0; block <= t.num_blocks; ++block) {
                // Iterate over every block, where we haven't found a key yet
               if (is_trailer_block(block)) {

                     if (!t.sectors[i].foundKeyA) {

                        memcpy(mp.mpa.abtKey,last_keyA,6);
                       if (!nfc_initiator_mifare_cmd(pnd, mca, block, &mp))
                        {
                             nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                        }
                       else {
                           memcpy(t.sectors[i].KeyA, mp.mpa.abtKey,6);
                           t.sectors[i].foundKeyA = true;
                       }
                      }
                     if (!t.sectors[i].foundKeyA) {
                         for (key_index = 0; key_index < ui->listWidget_keys->count(); key_index++) {
                                       memcpy(mp.mpa.abtKey, keys + (key_index*6), 6);
                                        if (!nfc_initiator_mifare_cmd(pnd,mca,block,&mp)) {
                                           nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                                        } else {
                                                // Save all information about successfull keyA authentization
                                                memcpy(t.sectors[i].KeyA, mp.mpa.abtKey,6);
                                                memcpy(last_keyA,mp.mpa.abtKey,6);
                                                t.sectors[i].foundKeyA = true;
                                                break;

                                        }
                                    }
                     }
                     if (!t.sectors[i].foundKeyA) {
                                            update_status(QString("Unkown Key A for sector %1: cannot dump memory").arg(i));
                                            succeed=0;
                                            break;

                                     }
                     if (!t.sectors[i].foundKeyB) {

                                        memcpy(mp.mpa.abtKey,last_keyB,6);
                                        if (!nfc_initiator_mifare_cmd(pnd, mcb, block, &mp))
                                        {
                                             nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);

                                        }
                                       else {
                                           memcpy(t.sectors[i].KeyB, mp.mpa.abtKey, sizeof(mp.mpa.abtKey));
                                           t.sectors[i].foundKeyB = true;
                                       }
                                   }
                     if (!t.sectors[i].foundKeyB) {
                          for (key_index = 0; key_index < ui->listWidget_keys->count(); key_index++) {
                                        memcpy(mp.mpa.abtKey, keys + (key_index*6), 6);

                                        if (!nfc_initiator_mifare_cmd(pnd,mcb,block,&mp)) {
                                                nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                                                // No success, try next block
                                                t.sectors[i].trailer = block;
                                        } else {
                                                memcpy(t.sectors[i].KeyB, mp.mpa.abtKey, sizeof(mp.mpa.abtKey));
                                                t.sectors[i].foundKeyB = true;
                                                memcpy(last_keyB,mp.mpa.abtKey,6);
                                                break;
                                        }
                                }
                      }
                     if (!t.sectors[i].foundKeyB) {
                          update_status(QString("Unkown Key B for sector %1: cannot dump memory").arg(i));
                           succeed=0;
                          break;

                      }
//
//                        nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                         // Save position of a trailer block to sector struct
               //         update_status(QString("Sector %1 keyA: %2 keyB: %3").arg(i).arg(b2QStringHex(t.sectors[i].KeyA,6)).arg(b2QStringHex(t.sectors[i].KeyB,6)));
                          t.sectors[i++].trailer = block;
                        }
                }
         if (succeed) {
                 i = t.num_sectors; // Sector counter
//               // Read all blocks
                 for (block = t.num_blocks; block >= 0; block--) {
                         is_trailer_block(block) ? i-- : i;
                         failure = true;
                         // Try A key, auth() + read()
                         memcpy(mp.mpa.abtKey, t.sectors[i].KeyA, sizeof(t.sectors[i].KeyA));
                         if (!nfc_initiator_mifare_cmd(pnd, MC_AUTH_A, block, &mp)) {
                                 mf_configure(pnd);
                                 mf_anticollision(t,pnd);
                         } else { // and Read
                                 if (nfc_initiator_mifare_cmd(pnd, MC_READ, block, &mp)) {
                                         mf_configure(pnd);
                                         mf_select_tag(pnd, &t.ti);
                                         failure = false;
                                 } else {
                                         // Error, now try read() with B key
                                         mf_configure(pnd);
                                         mf_anticollision(t,pnd);
                                         memcpy(mp.mpa.abtKey, t.sectors[i].KeyB, sizeof(t.sectors[i].KeyB));
                                         if (!nfc_initiator_mifare_cmd(pnd, MC_AUTH_B, block, &mp)) {
                                                 mf_configure(pnd);
                                                 mf_anticollision(t,pnd);
                                         } else { // and Read
                                                 if (nfc_initiator_mifare_cmd(pnd, MC_READ, block, &mp)) {
                                                         mf_configure(pnd);
                                                         mf_select_tag(pnd, &t.ti);
                                                         failure = false;
                                                 } else {
                                                         mf_configure(pnd);
                                                         mf_anticollision(t,pnd);
                                                 }
                                         }
                                 }
                         }
                         if (failure) update_status("Error!");
                         if (is_trailer_block(block)) {
                                 // Copy the keys over from our key dump and store the retrieved access bits
                                 memcpy(mtDump.amb[block].mbt.abtKeyA, t.sectors[i].KeyA,6);
                                 memcpy(mtDump.amb[block].mbt.abtKeyB,t.sectors[i].KeyB,6);
                                 if (!failure) memcpy(mtDump.amb[block].mbt.abtAccessBits,mp.mpd.abtData+6,4);
                         } else if (!failure) memcpy(mtDump.amb[block].mbd.abtData, mp.mpd.abtData,16);
                         memcpy(mp.mpa.abtUid,t.ti.nai.abtUid,4);
                 }

                     FILE * pfDump;
                     pfDump=fopen(fileName.toLatin1(), "wb");
                    // Finally save all keys + data to file
                    if (fwrite(&mtDump, 1, sizeof(mtDump), pfDump) != sizeof(mtDump)) fclose(pfDump);
                    fclose(pfDump);
                }
            free(t.sectors);
            update_status("Dumping Complete");
      }
          else {
              update_status("ERR:No output file selected!");
          }
         iTimer->start();
      }
    else { update_status("ERR:No Classic tag found!"); }


}
void MainWindow::savekeys(nfc_device_t* pnd) {
    if ((connected==true) && (tag_found==true) && (card_type != 4)) {
    iTimer->stop();
    mftag	t;
    int i=0,block,key_index;
    byte_t last_keyA[6];
    byte_t last_keyB[6];
    mifare_cmd mca;
    mifare_cmd mcb;
    mca = MC_AUTH_A;
    mcb = MC_AUTH_B;
    memcpy(mp.mpa.abtUid,nti.nai.abtUid,4);
    //1k  0x3f           //4k 0xff            //mini 0x13
    t.num_blocks = (card_type == 1) ? 0x3f : (card_type==2) ? 0xff : (card_type==3) ? 0x13 : 0 ;
    t.num_sectors = (card_type == 1) ? 16 : (card_type==2) ? 40 : (card_type==3) ? 5 : 0;
    t.sectors = (sector *) calloc(t.num_sectors, sizeof(sector));
    QString s=a(nti.nai.abtUid,nti.nai.szUidLen);
    s.append(" keys.txt");
    QFile::remove(s);
    QFile file(s);
    if (!file.open(QIODevice::Append | QIODevice::Text))
         return;
    QTextStream keyfile(&file);
    i = 0; // Sector counter
    nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);

    for (block = 0; block <= t.num_blocks; ++block) {
           // Iterate over every block, where we haven't found a key yet
          if (is_trailer_block(block)) {

                if (!t.sectors[i].foundKeyA) {

                   memcpy(mp.mpa.abtKey,last_keyA,6);
                  if (!nfc_initiator_mifare_cmd(pnd, mca, block, &mp))
                   {
                        nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                   }
                  else {
                      memcpy(t.sectors[i].KeyA, mp.mpa.abtKey,6);
                      t.sectors[i].foundKeyA = true;
                  }
                 }
                if (!t.sectors[i].foundKeyA) {
                    for (key_index = 0; key_index < ui->listWidget_keys->count(); key_index++) {
                                    memcpy(mp.mpa.abtKey, keys + (key_index*6), 6);
                                   if (!nfc_initiator_mifare_cmd(pnd,mca,block,&mp)) {
                                      nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                                   } else {
                                           // Save all information about successfull keyA authentization
                                           memcpy(t.sectors[i].KeyA, mp.mpa.abtKey,6);
                                           memcpy(last_keyA,mp.mpa.abtKey,6);
                                           t.sectors[i].foundKeyA = true;
                                           break;

                                   }
                               }
                }
                if (!t.sectors[i].foundKeyB) {

                                   memcpy(mp.mpa.abtKey,last_keyB,6);
                                   if (!nfc_initiator_mifare_cmd(pnd, mcb, block, &mp))
                                   {
                                        nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);

                                   }
                                  else {
                                      memcpy(t.sectors[i].KeyB, mp.mpa.abtKey, sizeof(mp.mpa.abtKey));
                                      t.sectors[i].foundKeyB = true;
                                  }
                              }
                if (!t.sectors[i].foundKeyB) {
                     for (key_index = 0; key_index < ui->listWidget_keys->count(); key_index++) {
                                   memcpy(mp.mpa.abtKey, keys + (key_index*6), 6);

                                   if (!nfc_initiator_mifare_cmd(pnd,mcb,block,&mp)) {
                                           nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                                           // No success, try next block
                                           t.sectors[i].trailer = block;
                                   } else {
                                           memcpy(t.sectors[i].KeyB, mp.mpa.abtKey, sizeof(mp.mpa.abtKey));
                                           t.sectors[i].foundKeyB = true;
                                           memcpy(last_keyB,mp.mpa.abtKey,6);
                                           break;
                                   }
                           }
                 }
                  keyfile << i <<",";
                if (!t.sectors[i].foundKeyA) {
                  keyfile << "UNKNOWN_KEY[A]";
                 }
                else keyfile <<  b2QStringHex(t.sectors[i].KeyA,6) ;
                keyfile << ",";
                if (!t.sectors[i].foundKeyB) {
                    keyfile << "UNKOWN_KEY[B]";
                } else  keyfile <<  b2QStringHex(t.sectors[i].KeyB,6);
                keyfile  << "\n";
             t.sectors[i++].trailer = block;
           }
      }
   file.close();
   update_status("All known keys saved");
   iTimer->start();
}
    else { update_status("Only For Mifare Classic Tags"); }
}


void MainWindow::on_pushButton_StopListening_clicked()
{
    if (connected==true) {
    if (ui->pushButton_StopListening->text()=="Stop Listening")   {
        iTimer->stop();
        ui->pushButton_StopListening->setText("Start Listening");
    }
    else {
        iTimer->start();
        ui->pushButton_StopListening->setText("Stop Listening");
        }
}
 }


void MainWindow::on_pushButton_2_clicked()
{
    //save keys to file!
    if ((connected==true) && (tag_found==true)) {
        savekeys(pnd);
    }
    else {
        update_status("No tag found!");
    }
}

void MainWindow::on_pushButton_WriteFrmDump_clicked()
{
    if ((connected==true) && (tag_found==true) && (card_type !=4)) {
          iTimer->stop();
          mftag		t;
          int i,key_index,succeed,failure;
          unsigned int block;
          byte_t last_keyA[6];
          byte_t last_keyB[6];
          mifare_cmd mca;
          mifare_cmd mcb;
          mifare_tag dump;
          mca = MC_AUTH_A;
          mcb = MC_AUTH_B;
          memcpy(mp.mpa.abtUid,nti.nai.abtUid,4);
          //1k  0x3f           //4k 0xff            //mini 0x13
          t.num_blocks = (card_type == 1) ? 0x3f : (card_type==2) ? 0xff : (card_type==3) ? 0x13 : 0 ;
          t.num_sectors = (card_type == 1) ? 16 : (card_type==2) ? 40 : (card_type==3) ? 5 : 0;
          t.sectors = (sector *) calloc(t.num_sectors, sizeof(sector));
          QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                          "",
                                      tr("Dump Files (*.dump)"));
          if (!fileName.isNull()) {
          if (t.sectors == NULL) {
              update_status("Cannot allocate memory for t.sectors\n");
         }
          FILE * pfDump;
          pfDump=fopen(fileName.toLatin1(), "rb");
          for (block = 0; block <= t.num_blocks;++block) {
             fread(dump.amb[block].mbd.abtData,16,1, pfDump);
          }
          fclose(pfDump);
         for (i = 0; i < (t.num_sectors); ++i) {
                 t.sectors[i].foundKeyA = t.sectors[i].foundKeyB = false;
         }

         i = 0; // Sector counter
         nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);

         for (block = 0; block <= t.num_blocks; ++block) {
                // Iterate over every block, where we haven't found a key yet
               if (is_trailer_block(block)) {

                     if (!t.sectors[i].foundKeyA) {

                        memcpy(mp.mpa.abtKey,last_keyA,6);
                       if (!nfc_initiator_mifare_cmd(pnd, mca, block, &mp))
                        {
                             nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                        }
                       else {
                           memcpy(t.sectors[i].KeyA, mp.mpa.abtKey,6);
                           t.sectors[i].foundKeyA = true;
                       }
                      }
                     if (!t.sectors[i].foundKeyA) {
                         for (key_index = 0; key_index < ui->listWidget_keys->count(); key_index++) {
                                       memcpy(mp.mpa.abtKey, keys + (key_index*6), 6);
                                        if (!nfc_initiator_mifare_cmd(pnd,mca,block,&mp)) {
                                           nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                                        } else {
                                                // Save all information about successfull keyA authentization
                                                memcpy(t.sectors[i].KeyA, mp.mpa.abtKey,6);
                                                memcpy(last_keyA,mp.mpa.abtKey,6);
                                                t.sectors[i].foundKeyA = true;
                                                break;

                                        }
                                    }
                     }
                     if (!t.sectors[i].foundKeyA) {
                                            update_status(QString("Unkown Key A for sector %1: cannot write dump").arg(i));
                                            succeed=0;
                                            break;

                                     }
                     if (!t.sectors[i].foundKeyB) {

                                        memcpy(mp.mpa.abtKey,last_keyB,6);
                                        if (!nfc_initiator_mifare_cmd(pnd, mcb, block, &mp))
                                        {
                                             nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);

                                        }
                                       else {
                                           memcpy(t.sectors[i].KeyB, mp.mpa.abtKey, sizeof(mp.mpa.abtKey));
                                           t.sectors[i].foundKeyB = true;
                                       }
                                   }
                     if (!t.sectors[i].foundKeyB) {
                          for (key_index = 0; key_index < ui->listWidget_keys->count(); key_index++) {
                                        memcpy(mp.mpa.abtKey, keys + (key_index*6), 6);

                                        if (!nfc_initiator_mifare_cmd(pnd,mcb,block,&mp)) {
                                                nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                                                // No success, try next block
                                                t.sectors[i].trailer = block;
                                        } else {
                                                memcpy(t.sectors[i].KeyB, mp.mpa.abtKey, sizeof(mp.mpa.abtKey));
                                                t.sectors[i].foundKeyB = true;
                                                memcpy(last_keyB,mp.mpa.abtKey,6);
                                                break;
                                        }
                                }
                      }
                     if (!t.sectors[i].foundKeyB) {
                          update_status(QString("Unkown Key B for sector %1: cannot write dump").arg(i));
                           succeed=0;
                          break;

                      }
//
//                        nfc_initiator_select_tag(pnd, NM_ISO14443A_106, NULL, 0, &nti);
                         // Save position of a trailer block to sector struct
               //         update_status(QString("Sector %1 keyA: %2 keyB: %3").arg(i).arg(b2QStringHex(t.sectors[i].KeyA,6)).arg(b2QStringHex(t.sectors[i].KeyB,6)));
                          t.sectors[i++].trailer = block;
                        }
                }
         if (succeed) {
                  i = t.num_sectors; // Sector counter
                  // Write all blocks
                  bUseKeyA=true;
                  // da sistemare!
                  for (block = t.num_blocks; block > 0; block--) {
                    write_classic_card(pnd,block,dump.amb[block].mbd.abtData,16);
                  }
                  bUseKeyA=false;
                  for (block = t.num_blocks; block > 0; block--) {
                     write_classic_card(pnd,block,dump.amb[block].mbd.abtData,16);
                  }
              }
// inspiegabilmente il codice sotto equivalente a quello sopra con la sola differenza che invece di leggere scrive non  funziona
// il reader non riesce ad autenticarsi al tag !

         //                  if (!nfc_initiator_select_tag(pnd, NM_ISO14443A_106, mp.mpa.abtUid, 4, NULL)) {
//                     update_status("No Tag selected");
//                  }
//
//
//                       for (block = t.num_blocks; block > 0; block--) {
//                         is_trailer_block(block) ? i-- : i;
//                         failure = true;
//                      //   memset(mp.mpd.abtData, '\0',16); //bzero function empties the data
//
//                         memcpy(mp.mpd.abtData,dump.amb[block].mbd.abtData,16); //copies content to mp
//                         // Try A key, auth() + write()
//                         memcpy(mp.mpa.abtKey, t.sectors[i].KeyA, sizeof(t.sectors[i].KeyA));
//                         if (!nfc_initiator_mifare_cmd(pnd,mca, block, &mp)) {
//                                 mf_configure(pnd);
//                                 mf_anticollision(t,pnd);
//                         } else { // and Read
//                                 if (nfc_initiator_mifare_cmd(pnd,MC_WRITE, block, &mp)) {
//                                         mf_configure(pnd);
//                                         mf_select_tag(pnd, &t.ti);
//                                         failure = false;
//                                 } else {
//                                         // Error, now try read() with B key
//                                         mf_configure(pnd);
//                                         mf_anticollision(t,pnd);
//                                         memcpy(mp.mpa.abtKey, t.sectors[i].KeyB, sizeof(t.sectors[i].KeyB));
//                                         if (!nfc_initiator_mifare_cmd(pnd,mcb,block,&mp)) {
//                                                 mf_configure(pnd);
//                                                 mf_anticollision(t,pnd);
//
//                                         } else { // and Read
//
//                                                 if (nfc_initiator_mifare_cmd(pnd,MC_WRITE, block, &mp)) {
//                                                         mf_configure(pnd);
//                                                         mf_select_tag(pnd, &t.ti);
//                                                         failure = false;
//                                                 } else {
//                                                         mf_configure(pnd);
//                                                         mf_anticollision(t,pnd);
//                                                 }
//                                         }
//                                 }
//                         }
//                         if (is_trailer_block(block)) {
//
//                             memcpy(t.sectors[i].KeyA,mp.mpd.abtData,6);
//                             memcpy(t.sectors[i].KeyB,mp.mpd.abtData+10,6);
//                            }
//                         memcpy(mp.mpa.abtUid,t.ti.nai.abtUid,4);
//
              //}

                // }


            free(t.sectors);
            read();
            update_status("Memory dumping completed");
      }
          else {
              update_status("ERR:No input file selected!");
          }
         iTimer->start();
      }
    else { update_status("ERR:No Classic tag found!"); }

}
