QT += network qml quickwidgets

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
    qml.qrc \
    qml/common/common.qrc

greaterThan(QT_MAJOR_VERSION, 5) {
    RESOURCES += qml/new/main.qrc
} else {
    lessThan(QT_MINOR_VERSION, 15) {
        RESOURCES += qml/old/main.qrc
    } else {
        RESOURCES += qml/new/main.qrc
    }
}

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
    msvc:LIBS += ws2_32.lib ole32.lib netapi32.lib user32.lib
    gcc:LIBS += -lws2_32 -lole32 -lnetapi32 -luser32
    HEADERS += ecwin7.h
    SOURCES += ecwin7.cpp
}

mac:ICON = dukto.icns

linux {
    ### libnotify
    CONFIG+=link_pkgconfig
    PKGCONFIG+=libnotify
    DEFINES+=NOTIFY_LIBNOTIFY
}
