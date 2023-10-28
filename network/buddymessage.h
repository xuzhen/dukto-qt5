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

#ifndef BUDDYMESSAGE_H
#define BUDDYMESSAGE_H

#include <QString>

class BuddyMessage
{
public:
    enum MSG_TYPE {
        MSG_INVALID              = 0x00,

        MSG_HELLO_BROADCAST      = 0x01,
        MSG_HELLO_UNICAST        = 0x02,
        MSG_GOODBYE              = 0x03,
        MSG_HELLO_PORT_BROADCAST = 0x04,
        MSG_HELLO_PORT_UNICAST   = 0x05,

        MSG_MAX = MSG_HELLO_PORT_UNICAST
    };

    BuddyMessage(MSG_TYPE type, quint16 port, const QString &signature) : type(type), port(port), signature(signature) {}
    BuddyMessage(const BuddyMessage &another) : type(another.type), port(another.type), signature(another.signature) {}

    inline bool isValid() const { return type != MSG_INVALID; }
    inline MSG_TYPE getType() const { return type; }
    inline quint16 getPort() const { return port; }
    inline const QString getSignature() const { return signature; }

    static BuddyMessage parse(const QByteArray &data);
    QByteArray serialize() const;

    inline static BuddyMessage goodbye() { return BuddyMessage(MSG_GOODBYE, 0, QString()); }
    inline static MSG_TYPE broadcastType(bool withPort) { return withPort ? MSG_HELLO_BROADCAST : MSG_HELLO_PORT_BROADCAST; }
    inline static MSG_TYPE unicastType(bool withPort) { return withPort ? MSG_HELLO_UNICAST : MSG_HELLO_PORT_UNICAST; }

private:
    MSG_TYPE type;
    quint16 port;
    QString signature;
};

#endif // BUDDYMESSAGE_H
