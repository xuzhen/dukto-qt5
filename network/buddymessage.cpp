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

#include "buddymessage.h"


BuddyMessage BuddyMessage::parse(const QByteArray &data) {
    if (data.isEmpty()) {
        return BuddyMessage(MSG_INVALID, 0, QString());
    }

    char type = data.at(0);
    if (type < 0 || type > MSG_MAX) {
        return BuddyMessage(MSG_INVALID, 0, QString());
    }

    quint16 port = 0;
    QString signature;
    switch (type) {
        case MSG_HELLO_BROADCAST:
        case MSG_HELLO_UNICAST:
            signature = QString::fromUtf8(data.mid(1));
            break;
        case MSG_GOODBYE:
            break;
        case MSG_HELLO_PORT_BROADCAST:
        case MSG_HELLO_PORT_UNICAST: {
            port = *(reinterpret_cast<const quint16*>(data.constData() + 1));
            signature = QString::fromUtf8(data.mid(1 + sizeof(quint16)));
            break;
        }
    }
    if ((type != MSG_GOODBYE && signature.isEmpty())
            || ((type == MSG_HELLO_PORT_BROADCAST || type == MSG_HELLO_PORT_UNICAST) && port == 0)) {
        return BuddyMessage(MSG_INVALID, 0, QString());
    }
    return BuddyMessage(static_cast<MSG_TYPE>(type), port, signature);
}

QByteArray BuddyMessage::serialize() const {
    QByteArray bytes;
    bytes.append(static_cast<char>(type));
    if (type == MSG_HELLO_PORT_BROADCAST || type == MSG_HELLO_PORT_UNICAST) {
        bytes.append(reinterpret_cast<const char *>(&port), sizeof(quint16));
    }
    if (type != MSG_GOODBYE) {
        bytes.append(signature.toUtf8());
    }
    return bytes;
}
