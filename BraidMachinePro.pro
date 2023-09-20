QT += core sql network serialport gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
CONFIG += c++14
QMAKE_CXXFLAGS_RELEASE = -Od -ZI -MD
QMAKE_LFLAGS_RELEASE = /DEBUG /INCREMENTAL:NO

SOURCES += \
        gdatafactory.cpp \
        main.cpp \
        tcpclient.cpp \
        udpservice.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    gdatafactory.h \
    tcpclient.h \
    udpservice.h


INCLUDEPATH += .\QsLog-master

DEFINES += QSLOG_IS_SHARED_LIBRARY_IMPORT
DEFINES += QS_LOG_LINE_NUMBERS

win32: LIBS += -L$$PWD/lib/x64/ -lQsLog2

INCLUDEPATH += $$PWD/lib/x64
DEPENDPATH += $$PWD/lib/x64

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/x64/QsLog2.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/lib/x64/libQsLog2.a
