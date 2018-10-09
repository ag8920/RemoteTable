#-------------------------------------------------
#
# Project created by QtCreator 2018-09-05T14:03:47
#
#-------------------------------------------------

QT       += core gui widgets serialport

TARGET = GyroCompas
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

CONFIG(debug, debug|release) {
    DESTDIR = $$OUT_PWD/debug
} else {
    DESTDIR = $$OUT_PWD/release
}
CONFIG(debug, debug|release) {
QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/debug
} else{
QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/release
}

#CONFIG(debug, debug|release)
MOC_DIR=moc
RCC_DIR=rcc
UI_DIR=ui
OBJECTS_DIR=obj


SOURCES += \
        main.cpp \
        widget.cpp \
    TableDev/tabledevice.cpp \
    GyroDev/gyrodevice.cpp \
    comport/settingsdialog/settingsdialog.cpp \
    comport/comport.cpp \
    Console/console.cpp \
    GyroDev/gyrodata.cpp \
    comport/slipprotocol.cpp \
    ModelData/tablemodel.cpp \
    Timer/ptimer.cpp \
    loger/loger.cpp


HEADERS += \
        widget.h \
    TableDev/tabledevice.h \
    GyroDev/gyrodevice.h \
    comport/settingsdialog/settingsdialog.h \
    comport/comport.h \
    Console/console.h \
    GyroDev/gyrodata.h \
    comport/slipprotocol.h \
    ModelData/tablemodel.h \
    Timer/ptimer.h \
    loger/loger.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#RC_FILE = resources.rc

FORMS += \
    comport/settingsdialog/settingsdialog.ui

RESOURCES += \
    resource.qrc

RC_FILE = resources.rc

DISTFILES +=


