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

#include "settings.h"
#include "platform.h"

#include <QSettings>
#include <QDir>
#include "theme.h"

Settings::Settings(QObject *parent) :
    QObject(parent), mSettings("msec.it", "Dukto")
{
}

QString Settings::currentPath()
{
    // Retrieve the last saved path (if any)
    QString path = mSettings.value("DestPath", "").toString();

    // Check if exists
    if (!path.isEmpty() && (QDir(path).exists()))
            return path;

    // Else return the default path for this platform
    path = Platform::getDefaultPath();
    if (QDir(path).exists())
        return path;
    else
        return ".";
}

void Settings::savePath(const QString &path)
{
    // Save the new path
    mSettings.setValue("DestPath", path);
    mSettings.sync();
}

void Settings::saveWindowGeometry(const QByteArray &geo)
{
    mSettings.setValue("WindowPosAndSize", geo);
    mSettings.sync();
}

QByteArray Settings::windowGeometry()
{
    return mSettings.value("WindowPosAndSize").toByteArray();
}

void Settings::saveThemeColor(const QString &color)
{
    mSettings.setValue("ThemeColor", color);
    mSettings.sync();
}

QString Settings::themeColor()
{
    return mSettings.value("ThemeColor", Theme::DEFAULT_THEME_COLOR).toString();
}

void Settings::saveShowTermsOnStart(bool show)
{
    mSettings.setValue("R5/ShowTermsOnStart", show);
    mSettings.sync();
}

bool Settings::showTermsOnStart()
{
    return mSettings.value("R5/ShowTermsOnStart", true).toBool();
}

QString Settings::buddyName()
{
    // Retrieve the last saved name (if any)
    return mSettings.value("BuddyName", "User").toString();

}

void Settings::saveBuddyName(const QString &name)
{
    // Save the new name
    mSettings.setValue("BuddyName", name);
    mSettings.sync();
}

bool Settings::notificationEnabled() {
    return mSettings.value("Notification", false).toBool();
}

void Settings::saveNotificationEnabled(bool enabled) {
    mSettings.setValue("Notification", enabled);
    mSettings.sync();
}
