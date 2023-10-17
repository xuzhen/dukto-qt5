/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 * Copyright (C) 2015 Arthur Zamarin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <QApplication>
#include <QCommandLineParser>

#include "guibehind.h"
#include "duktowindow.h"

#ifndef MOBILE_APP
#include "systemtray.h"
#endif

#ifdef SINGLE_APP
#include <singleapplication.h>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    qputenv("QML_ENABLE_TEXT_IMAGE_CACHE", "true");
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qputenv("QSG_RHI_BACKEND", "opengl");
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 4, 0) && QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    if (qEnvironmentVariableIsEmpty("QT_DEVICE_PIXEL_RATIO")) {
        qputenv("QT_DEVICE_PIXEL_RATIO", "auto");
    }
#endif

#ifndef SINGLE_APP
    QApplication app(argc, argv);
#else
    // Check for single running instance
    SingleApplication app(argc, argv, true);
    if (app.isSecondary()) {
#ifdef Q_OS_WIN
        AllowSetForegroundWindow(static_cast<DWORD>(app.primaryPid()));
#endif
        app.sendMessage(QByteArray("A"));
        return 0;
    }
#endif

#ifndef MOBILE_APP
    QCommandLineParser parser;
    parser.setApplicationDescription("Dukto is a simple, fast and multi-platform file transfer tool for LAN users.");
    parser.addHelpOption();
    QCommandLineOption hideOption(QStringList() << "H" << "hide", "Hide when launched");
    parser.addOption(hideOption);
    parser.process(app);
#endif

    GuiBehind gb;
    app.installEventFilter(&gb);
    
    DuktoWindow viewer(&gb);
#ifdef SINGLE_APP
    QObject::connect(&app, &SingleApplication::receivedMessage, &viewer, &DuktoWindow::activateWindow);
#endif

#ifndef MOBILE_APP
    SystemTray tray(viewer);
    gb.setViewer(&viewer, &tray);
    viewer.setVisible(!parser.isSet(hideOption));
    tray.show();
#else
    gb.setViewer(&viewer, nullptr);
    viewer.showMaximized();
#endif

    return app.exec();
}
