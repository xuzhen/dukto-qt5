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
#include "systemtray.h"

#include "settings.h"
#include "guibehind.h"
#include "duktowindow.h"

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
    qputenv("QSG_RHI_BACKEND", "gl");
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
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Dukto is a simple, fast and multi-platform file transfer tool for LAN users.");
    parser.addHelpOption();
    QCommandLineOption hideOption(QStringList() << "H" << "hide", "Hide when launched");
    parser.addOption(hideOption);
    parser.process(app);

    Settings settings;

    GuiBehind gb(&settings);
    app.installEventFilter(&gb);
    
    DuktoWindow viewer(&gb, &settings);
#ifdef SINGLE_APP
    QObject::connect(&app, &SingleApplication::receivedMessage, [&viewer]()->void {
        if (viewer.isMinimized()) {
            viewer.showNormal();
        }
        viewer.activateWindow();
    });
#endif

    SystemTray tray(viewer);
    tray.show();

    gb.setViewer(&viewer, &tray);
    viewer.setVisible(!parser.isSet(hideOption));
    
    return app.exec();
}
