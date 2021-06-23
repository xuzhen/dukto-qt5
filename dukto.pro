QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dukto
TEMPLATE = app

VERSION = 6.0.0

unix {
    target.path = /usr/bin

    icon.path = /usr/share/pixmaps
    icon.files = dukto.png
    INSTALLS += icon

    desktop.path = /usr/share/applications/
    desktop.files = dukto.desktop
    INSTALLS += desktop
}

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    guibehind.cpp \
    platform.cpp \
    buddylistitemmodel.cpp \
    duktoprotocol.cpp \
    ipaddressitemmodel.cpp \
    recentlistitemmodel.cpp \
    settings.cpp \
    destinationbuddy.cpp \
    duktowindow.cpp \
    theme.cpp \
    updateschecker.cpp \
    systemtray.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    guibehind.h \
    platform.h \
    buddylistitemmodel.h \
    duktoprotocol.h \
    peer.h \
    ipaddressitemmodel.h \
    recentlistitemmodel.h \
    settings.h \
    destinationbuddy.h \
    duktowindow.h \
    theme.h \
    updateschecker.h \
    systemtray.h

RESOURCES += \
    qml.qrc

#//FIXME
#UPDATER
DEFINES += SINGLE_APP
contains(DEFINES, SINGLE_APP) {
    include(modules/SingleApplication/singleapplication.pri)
    DEFINES += QAPPLICATION_CLASS=QApplication
}

OTHER_FILES += CMakeLists.txt

win32 {
    RC_FILE = dukto.rc
    LIBS += libWs2_32 libole32 libNetapi32
    HEADERS += ecwin7.h
    SOURCES += ecwin7.cpp
}

mac:ICON = dukto.icns

### libnotify
CONFIG+=link_pkgconfig
PKGCONFIG+=libnotify
DEFINES+=NOTIFY_LIBNOTIFY
