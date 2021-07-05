/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
public:
    explicit Settings(QObject *parent = nullptr);
    QString currentPath();
    void savePath(const QString &path);
    void saveWindowGeometry(const QByteArray &geo);
    QByteArray windowGeometry();
    void saveThemeColor(const QString &color);
    QString themeColor();
    void saveShowTermsOnStart(bool show);
    bool showTermsOnStart();
    QString buddyName();
    void saveBuddyName(const QString &name);
    bool notificationEnabled();
    void saveNotificationEnabled(bool enabled);
    bool closeToTrayEnabled();
    void saveCloseToTrayEnabled(bool enabled);

private:
    QSettings mSettings;

};

#endif // SETTINGS_H
