# -------------------------------------------------
# Project created by QtCreator 2010-06-13T20:51:06
# -------------------------------------------------
TARGET = Libnfc_gui
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h \
    nfc/nfc-messages.h \
    nfc/nfc.h \
    nfc/mifareultag.h \
    nfc/mifaretag.h \
    nfc/bitutils.h \
    nfc/nfc-types.h \
    stdint/stdint.h \
    stdint/stdbool.h \
    stdint/inttypes.h \
    ui_mainwindow.h
LIBS += C:\Users\admin\Desktop\Libnfc_gui\libnfc.dll
#LIBS += D:\Projects\QT\LIBNFC_console\libnfc.lib
FORMS += mainwindow.ui
RC_FILE += rfid.rc
