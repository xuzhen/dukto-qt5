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

#include "recentlistitemmodel.h"

#include <QDateTime>
#include <QLocale>

RecentListItemModel::RecentListItemModel() :
    QStandardItemModel(nullptr)
{
    QHash<int, QByteArray> roleNames;
    roleNames[Name] = "name";
    roleNames[Value] = "value";
    roleNames[Type] = "type";
    roleNames[TypeIcon] = "typeIcon";
    roleNames[DateTime] = "dateTime";
    roleNames[Sender] = "sender";
    roleNames[Size] = "size";
    setItemRoleNames(roleNames);
}

const double BYTES_TO_KB = 1.0 / 1024;
const double BYTES_TO_MB = 1.0 / 1048576;

void RecentListItemModel::addRecent(const QString &name, const QString &value, const QString &type, const QString &sender, qint64 size)
{
    QStandardItem* it = new QStandardItem();

    // Format timestamp
    QDateTime now = QDateTime::currentDateTime();
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QString datetime = QLocale::system().toString(now, QLocale::ShortFormat);
#else
    QString datetime = now.toString(Qt::SystemLocaleShortDate);
#endif

    QString sizeFormatted;
    if (size >= 0) {
        if (size < 1024)
            sizeFormatted = QString::number(size) + " B";
        else if (size < 1048576)
            sizeFormatted = QString::number(size * BYTES_TO_KB, 'f', 1) + " KB";
        else
            sizeFormatted = QString::number(size * BYTES_TO_MB, 'f', 1) + " MB";
    }

    // Icon
    if (type == "text")
        it->setData("RecentText.png", RecentListItemModel::TypeIcon);
    else if (type == "file")
        it->setData("RecentFile.png", RecentListItemModel::TypeIcon);
    else
        it->setData("RecentFiles.png", RecentListItemModel::TypeIcon);

    if (type == "file") {
        it->setData(name + " (" + sizeFormatted + ")", RecentListItemModel::Name);
    } else {
        it->setData(name, RecentListItemModel::Name);
    }

    it->setData(value, RecentListItemModel::Value);
    it->setData(type, RecentListItemModel::Type);
    it->setData(datetime, RecentListItemModel::DateTime);
    it->setData(sender, RecentListItemModel::Sender);
    it->setData(sizeFormatted, RecentListItemModel::Size);
    insertRow(0, it);
}
