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

#include "duktoprotocol.h"

#if defined(Q_OS_WIN)
    #include <windows.h>
#endif

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QNetworkInterface>
#include <QTimer>

#include "network/messenger.h"
#include "network/receiver.h"
#include "network/sender.h"
#include "platform.h"

#ifdef Q_OS_ANDROID
#include "androidutils.h"
#endif

#define DEFAULT_UDP_PORT 4644
#define DEFAULT_TCP_PORT 4644

enum MSG_TYPE {
    MSG_HELLO_BROADCAST = 0x01,
    MSG_HELLO_UNICAST = 0x02,
    MSG_GOODBYE = 0x03,
    MSG_HELLO_PORT_BROADCAST = 0x04,
    MSG_HELLO_PORT_UNICAST = 0x05
};

DuktoProtocol::DuktoProtocol(QObject *parent)
    : QObject(parent), mLocalTcpPort(DEFAULT_TCP_PORT)
{
}

DuktoProtocol::~DuktoProtocol()
{
    closeServers();
    delete mMessenger;
    delete mTcpServer;
}

bool DuktoProtocol::setupUdpServer(quint16 port)
{
    if (mMessenger == nullptr) {
        mMessenger = new Messenger(DEFAULT_UDP_PORT, this);
        connect(mMessenger, &Messenger::buddyFound, this, &DuktoProtocol::peerListAdded, Qt::QueuedConnection);
        connect(mMessenger, &Messenger::buddyGone, this, &DuktoProtocol::peerListRemoved, Qt::QueuedConnection);
    }
    return mMessenger->start(port);
}

bool DuktoProtocol::setupTcpServer(quint16 port)
{
    if (mTcpServer == nullptr) {
        mTcpServer = new QTcpServer(this);
    }
    if (mLocalTcpPort != port && mTcpServer->isListening()) {
        mLocalTcpPort = port;
        mTcpServer->close();
    }
    if (mTcpServer->isListening() == false && mTcpServer->listen(QHostAddress::AnyIPv4, mLocalTcpPort) == false) {
        return false;
    }
    connect(mTcpServer, &QTcpServer::newConnection, this, &DuktoProtocol::newIncomingConnection, Qt::UniqueConnection);
    return true;
}

void DuktoProtocol::closeServers() {
    if (mMessenger != nullptr) {
        mMessenger->stop();
    }
    if (mTcpServer != nullptr) {
        mTcpServer->close();
    }
}

void DuktoProtocol::greeting() {
    if (mMessenger != nullptr) {
        mMessenger->sayHello();
    }
}

// Richiesta connessione TCP in ingresso
void DuktoProtocol::newIncomingConnection()
{
    // Recieve connection
    QTcpSocket* s = mTcpServer->nextPendingConnection();
    if(s == nullptr) return;

    if (mReceiver != nullptr || mSender != nullptr) {
        s->close();
        return;
    }

    mReceiver = new Receiver(s, mDestDir, this);
    connect(mReceiver, &Receiver::progress, this, &DuktoProtocol::transferStatusUpdate);
    connect(mReceiver, &Receiver::dirReceived, this, &DuktoProtocol::receiveDirCompleted);
    connect(mReceiver, &Receiver::fileReceived, this, &DuktoProtocol::receiveFileCompleted);
    connect(mReceiver, &Receiver::textReceived, this, &DuktoProtocol::receiveTextCompleted);
    connect(mReceiver, &Receiver::aborted, this, [this](const QString &error) {
        mReceiver->deleteLater();
        mReceiver = nullptr;
        emit receiveAborted(error);
    });
    connect(mReceiver, &Receiver::completed, this, [this]() {
        mReceiver->deleteLater();
        mReceiver = nullptr;
        emit receiveCompleted();
    });

    // Update GUI
    emit receiveStarted(s->peerAddress().toString());
}

void DuktoProtocol::createSender(const QString &ipDest, qint16 port) {
    mSender = new Sender(ipDest, port);
    connect(mSender, &Sender::progress, this, &DuktoProtocol::transferStatusUpdate);
    connect(mSender, &Sender::completed, this, [this]() {
        mSender->deleteLater();
        mSender = nullptr;
        emit sendFileComplete();
    });
    connect(mSender, &Sender::aborted, this, [this](const QString &error) {
        mSender->deleteLater();
        mSender = nullptr;
        emit sendFileError(error);
    });
}

void DuktoProtocol::sendFile(const QString &ipDest, qint16 port, const QStringList &files)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Verifica altre attività in corso
    if (mReceiver != nullptr || mSender != nullptr) {
        return;
    }

    createSender(ipDest, port);
    mSender->sendFiles(files);
}

void DuktoProtocol::sendText(const QString &ipDest, qint16 port, const QString &text)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Verifica altre attività in corso
    if (mReceiver != nullptr || mSender != nullptr) return;

    createSender(ipDest, port);
    mSender->sendText(text);
}

void DuktoProtocol::sendScreen(const QString &ipDest, qint16 port, const QString &path)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Verifica altre attività in corso
    if (mReceiver != nullptr || mSender != nullptr) return;

    createSender(ipDest, port);
    mSender->sendFile(path, "Screenshot.jpg");
}

// Interrompe un trasferimento in corso (utilizzabile solo lato invio)
void DuktoProtocol::abortCurrentTransfer()
{
    // Abort current connection
    if (mSender != nullptr) {
        mSender->abort();
        emit sendFileAborted();
        mSender = nullptr;
    } else if (mReceiver != nullptr) {
        mReceiver->abort();
        emit receiveAborted(QString());
        mReceiver = nullptr;
    }
}

// Aggiorna il buddy name dell'utente locale
void DuktoProtocol::updateBuddy()
{
    if (mMessenger != nullptr) {
        mMessenger->sayGoodbye();
        mMessenger->sayHello();
    }
}

void DuktoProtocol::setDestDir(const QString &dir) {
#ifdef Q_OS_ANDROID
    mDestDir = dir;
#else
    mDestDir = QDir::cleanPath(dir);
    if (!mDestDir.endsWith(QChar('/'))) {
        mDestDir.append(QChar('/'));
    }
#endif
}
