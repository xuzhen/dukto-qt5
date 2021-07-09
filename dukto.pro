QT += network qml quickwidgets
linux:!android:QT += dbus
CONFIG += c++11

#============Compiling Options=============

# Allow only one instance
DEFINES += SINGLE_APP

# FIXME: Updater no longer works
#DEFINES += UPDATER

# Use libnotify for notifications (Linux only)
DEFINES += NOTIFY_LIBNOTIFY

#==========================================

android: {
    DEFINES -= NOTIFY_LIBNOTIFY
    DEFINES -= SINGLE_APP
    DEFINES += MOBILE_APP
}
!linux: {
    DEFINES -= NOTIFY_LIBNOTIFY
}

TARGET = dukto
TEMPLATE = app

DEFINES += UNICODE

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

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
    systemtray.h

RESOURCES += \
    qml.qrc \
    qml/common/common.qrc

greaterThan(QT_MAJOR_VERSION, 5) {
    RESOURCES += qml/new/main.qrc
} else:lessThan(QT_MINOR_VERSION, 14) {
    RESOURCES += qml/old/main.qrc
} else {
    RESOURCES += qml/new/main.qrc
}

contains(DEFINES, UPDATER) {
    SOURCES += updateschecker.cpp
    HEADERS += updateschecker.h
}

contains(DEFINES, SINGLE_APP) {
    include(modules/SingleApplication/singleapplication.pri)
    DEFINES += QAPPLICATION_CLASS=QApplication
}

OTHER_FILES += CMakeLists.txt

win32 {
    RC_FILE = dukto.rc
    msvc:LIBS += ws2_32.lib ole32.lib user32.lib
    gcc:LIBS += -lws2_32 -lole32 -luser32
    HEADERS += ecwin7.h
    SOURCES += ecwin7.cpp
}

mac:ICON = dukto.icns

contains(DEFINES, NOTIFY_LIBNOTIFY) {
    CONFIG+=link_pkgconfig
    PKGCONFIG+=libnotify
}
