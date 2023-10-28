/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2021 Xu Zhen
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

#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>
#include <QHash>

#include "peer.h"

class QUdpSocket;
class QHostAddress;
class BuddyMessage;
class QNetworkInterface;

#ifdef Q_OS_ANDROID
class AndroidMulticastLock;
#endif

class Messenger : public QObject
{
    Q_OBJECT
public:
    explicit Messenger(quint16 defaultPort, QObject *parent = nullptr);
    ~Messenger();

    bool start(quint16 listenPort, QString &error);
    void stop();
    void sayHello();
    void sayHello(const QHostAddress &target, quint16 port);
    void sayGoodbye();

signals:
    void buddyFound(Peer peer);
    void buddyGone(Peer peer);

private slots:
    void processDatagram();

private:
    void processMessage(const BuddyMessage &message, const QHostAddress &sender);
    void broadcastMessage(const BuddyMessage &message);
    void sendPacket(const QByteArray &data, const QHostAddress &target, quint16 port);
    QString getSystemSignature();

    QUdpSocket *socket;
    quint16 localPort = 0;
    const quint16 protocolDefaultPort;

    QHash<QHostAddress, Peer> peers;
    QHash<QHostAddress, int> localAddrs;
    QHash<QHostAddress, QNetworkInterface> localIfaces;

    // on Android, an interface created by some VPN apps may cause broadcast storm
    QList<QHostAddress> badAddrs;

#ifdef Q_OS_ANDROID
    AndroidMulticastLock *lock = nullptr;
#endif
};


#endif // MESSENGER_H
