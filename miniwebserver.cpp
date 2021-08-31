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

#include "miniwebserver.h"

#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QImage>
#include <QBuffer>
#include <QRegularExpression>

#include "platform.h"

MiniWebServer::MiniWebServer(int port)
{
    // Load and convert avatar image
    QString path = Platform::getAvatarPath();
    if (!path.isEmpty()) {
        QImage img(path);
        QImage scaled = img.scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QBuffer tmp(&mAvatarData);
        tmp.open(QIODevice::WriteOnly);
        scaled.save(&tmp, "PNG");

        // Start server
        listen(QHostAddress::AnyIPv4, port);
    }
}

void MiniWebServer::incomingConnection(qintptr handle)
{
    QTcpSocket* s = new QTcpSocket(this);
    connect(s, &QTcpSocket::readyRead, this, &MiniWebServer::readClient, Qt::QueuedConnection);
    connect(s, &QTcpSocket::disconnected, s, &QTcpSocket::deleteLater, Qt::QueuedConnection);
    s->setSocketDescriptor(handle);
}

void MiniWebServer::readClient()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegularExpression("[ \r\n][ \r\n]*"));
        if (tokens.at(0) == "GET" && (tokens.at(1) == "/" || tokens.at(1) == "/dukto/avatar")) {

            QTextStream os(socket);
            os.setAutoDetectUnicode(true);
            os << "HTTP/1.0 200 OK\r\n"
                "Content-Type: image/png\r\n"
                "Content-Length: " << mAvatarData.size() << "\r\n"
                "\r\n";
            os.flush();

            QDataStream ds(socket);
            ds.writeRawData(mAvatarData.data(), mAvatarData.size());

        }
        socket->close();
    }
}
