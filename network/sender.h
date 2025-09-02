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

#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QAbstractSocket>
#include "filedata.h"

class QTcpSocket;

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(const QString &dest, quint16 port, QObject *parent = nullptr);
    ~Sender();

    void sendFiles(const QStringList &paths);
    void sendFile(const QString &path, const QString &name = QString());
    void sendText(const QString &text);
    void abort();

signals:
    void started(qint64 totalSize);
    void progress(qint64 total, qint64 sent);
    void itemProgress(qint64 total, qint64 current, QString name);
    void completed();
    void aborted(QString error);

private slots:
    void sendData();
    void connectionError(QAbstractSocket::SocketError error);

private:
    void reportError(const QString &error);

    QTcpSocket *socket;
    QString dest;
    quint16 port;

    QList<FileData> filesToSend;
    qint64 totalElements = 0;
    int currentFileIndex = 0;
    FileData *currentFile = nullptr;
    QByteArray textToSend;
    qint64 totalBytes = 0;
    qint64 totalBytesSent = 0;

    enum SEND_PHASE {
        PHASE_TOTAL_ELEMENTS_AND_SIZE,
        PHASE_ELEMENT_NAME_AND_SIZE,
        PHASE_ELEMENT_DATA,
        PHASE_FINALIZATION,
    } sendStatus = PHASE_TOTAL_ELEMENTS_AND_SIZE;

    static QByteArray textElementName;

#ifdef Q_OS_ANDROID
    volatile AndroidScreenOn screenOn;
#endif
};

#endif // SENDER_H
