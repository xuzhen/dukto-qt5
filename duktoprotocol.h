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

#ifndef DUKTOPROTOCOL_H
#define DUKTOPROTOCOL_H

#include <QObject>

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QHash>
#include <QFile>
#include <QStringList>

#include "peer.h"

class Messenger;
class Receiver;
class Sender;

class DuktoProtocol : public QObject
{
    Q_OBJECT

public:
    DuktoProtocol(QObject *parent = nullptr);
    virtual ~DuktoProtocol();
    bool setupUdpServer(quint16 port, QString &error);
    bool setupTcpServer(quint16 port, QString &error);
    void closeServers();
    void greeting();
    void sendFile(const QString &ipDest, qint16 port, const QStringList &files);
    void sendText(const QString &ipDest, qint16 port, const QString &text);
    void sendScreen(const QString &ipDest, qint16 port, const QString &path);
    void abortCurrentTransfer();
    void updateBuddy();
    void setDestDir(const QString &dir);
    
private slots:
    void newIncomingConnection();

signals:
     void peerListAdded(Peer peer);
     void peerListRemoved(Peer peer);
     void sendFileComplete();
     void sendFileError(QString error);
     void sendFileAborted();
     void receiveStarted(QString senderIp);
     void receiveCompleted();
     void receiveAborted(QString error);
     void receiveFileCompleted(QString name, QString path, qint64 size);
     void receiveDirCompleted(QString name, QString path);
     void receiveTextCompleted(QString text);
     void transferStatusUpdate(qint64 total, qint64 partial);

private:
    void createSender(const QString &ipDest, qint16 port);

    Messenger *mMessenger = nullptr;
    Receiver *mReceiver = nullptr;
    Sender *mSender = nullptr;
    QTcpServer *mTcpServer = nullptr;           // Socket TCP attesa dati
    QTcpSocket *mCurrentSocket = nullptr;       // Socket TCP dell'attuale trasferimento file
    qint16 mLocalTcpPort;

    QString mDestDir;
};

#endif // DUKTOPROTOCOL_H
