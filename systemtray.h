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

#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QSystemTrayIcon>
#include "duktowindow.h"

class SystemTray : public QSystemTrayIcon
{
        Q_OBJECT
    public:
        explicit SystemTray(DuktoWindow& window, Settings *settings, QObject *parent = nullptr);
        ~SystemTray();

    public slots:
        void received_file(const QString &name, const QString &path, qint64 size);
        void received_folder(const QString &name, const QString &path);
        void received_text(const QString &text);

    private slots:
        void on_activated(QSystemTrayIcon::ActivationReason reason);

    private:
        DuktoWindow& window;
        Settings *settings;

        void notify(const QString &title, const QString &body);
};

#endif // SYSTEMTRAY_H
