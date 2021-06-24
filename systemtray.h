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
        SystemTray(DuktoWindow& window, QObject *parent = 0);
        ~SystemTray();

    public slots:
        void received_file(QStringList *files, qint64);
        void received_text(QString *text, qint64);

    private slots:
        void on_activated(QSystemTrayIcon::ActivationReason reason);

    private:
        DuktoWindow& window;

        void notify(QString title, QString body);
};

#endif // SYSTEMTRAY_H
