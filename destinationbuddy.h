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

#ifndef DESTINATIONBUDDY_H
#define DESTINATIONBUDDY_H

#include <QObject>

class QStandardItem;

class DestinationBuddy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ip READ ip NOTIFY ipChanged)
    Q_PROPERTY(qint16 port READ port NOTIFY portChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(QString system READ system NOTIFY systemChanged)
    Q_PROPERTY(QString platform READ platform NOTIFY platformChanged)
    Q_PROPERTY(QString genericAvatar READ genericAvatar NOTIFY genericAvatarChanged)
    Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)
    Q_PROPERTY(QString osLogo READ osLogo NOTIFY osLogoChanged)
    Q_PROPERTY(QString showBack READ showBack NOTIFY showBackChanged)

public:
    explicit DestinationBuddy(QObject *parent = nullptr);
    inline QString ip() { return mIp; }
    inline qint16 port() { return mPort; }
    inline QString username() { return mUsername; }
    inline QString system() { return mSystem; }
    inline QString platform() { return mPlatform; }
    inline QString genericAvatar() { return mGenericAvatar; }
    inline QString avatar() { return mAvatar; }
    inline QString osLogo() { return mOsLogo; }
    inline QString showBack() { return mShowBack; }
    void fillFromItem(QStandardItem *item);
    // void setAsRemoteBuddy(QString ip);

signals:
    void ipChanged();
    void portChanged();
    void usernameChanged();
    void systemChanged();
    void platformChanged();
    void genericAvatarChanged();
    void avatarChanged();
    void osLogoChanged();
    void showBackChanged();

public slots:

private:
    QString mIp;
    qint16 mPort = 0;
    QString mUsername;
    QString mSystem;
    QString mPlatform;
    QString mGenericAvatar;
    QString mAvatar;
    QString mOsLogo;
    QString mShowBack;
};

#endif // DESTINATIONBUDDY_H
