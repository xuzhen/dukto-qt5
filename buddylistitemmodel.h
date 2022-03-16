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

#ifndef BUDDYLISTITEMMODEL_H
#define BUDDYLISTITEMMODEL_H

#include <QStandardItemModel>

class Peer;
class QUrl;

class BuddyListItemModel : public QStandardItemModel
{
public:
    BuddyListItemModel();
    void addMeElement();
    void addIpElement();
    void addBuddy(const QString &ip, qint16 port, const QString &username, const QString &system, const QString &platform, const QUrl &avatarPath);
    void addBuddy(const Peer &peer);
    void removeBuddy(const QString &ip);
    void clearBuddies();
    void showSingleBack(int idx);
    void updateMeElement();
    QString getMeGenericAvatar();
    QString buddyNameByIp(const QString &ip);
    QStandardItem* buddyByIp(const QString &ip);
    QString fistBuddyIp();

    enum BuddyRoles {
        Ip = Qt::UserRole + 1,
        Port,
        Username,
        System,
        Platform,
        GenericAvatar,
        Avatar,
        OsLogo,
        ShowBack
    };

private:
    QHash<QString, QStandardItem*> mItemsMap;
    QStandardItem* mMeItem = nullptr;
};

#endif // BUDDYLISTITEMMODEL_H
