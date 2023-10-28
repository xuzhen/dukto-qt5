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

#ifndef RECEIVER_H
#define RECEIVER_H

#include <QTcpSocket>
#include <QMap>

#ifdef Q_OS_ANDROID
class AndroidContentWriter;
class AndroidScreenOn;
#else
class QFile;
#endif

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QTcpSocket *socket, const QString &destDir, QObject *parent = nullptr);
    ~Receiver();

    void abort();

signals:
    void started(qint64 totalSize);
    void progress(qint64 total, qint64 received);
    void completed();
    void aborted(QString error);
    void dirReceived(QString name, QString path);
    void fileReceived(QString name, QString path, qint64 size);
    void textReceived(QString text);

private slots:
    void processData();
    void connectionError(QAbstractSocket::SocketError error);

private:
    void endSession();
    void terminateSession(const QString &error);
    void terminateConnection();
    bool prepareFilesystem();
    QString getNewPath(const QString &originaPath);
    QString getNewFileName(const QString &parentDir, const QString &originalName);

    QTcpSocket *socket;

    QString destDir;

    qint64 sessionElements = 0;
    qint64 sessionBytes = 0;

    qint64 sessionElementsReceived = 0;
    qint64 sessionBytesReceived = 0;

    QByteArray readBuffer;

    QString currentElementName;
    qint64 currentElementBytes = 0;
    qint64 currentElementReceived = 0;
    enum ELEMENT_TYPE {
        FILE_ELEMENT,
        DIR_ELEMENT,
        TEXT_ELEMENT
    } currentElementType = FILE_ELEMENT;

    QString currentTopElementName;
    QString currentTopElementPath;

    QMap<QString,QString> dirNameMap;
#ifdef Q_OS_ANDROID
    AndroidContentWriter *currentFile = nullptr;
#else
    QFile *currentFile = nullptr;
#endif

    enum RECV_PHASE {
        PHASE_TOTAL_ELEMENTS,
        PHASE_TOTAL_SIZE,
        PHASE_ELEMENT_NAME,
        PHASE_ELEMENT_SIZE,
        PHASE_ELEMENT_DATA
    } recvStatus = PHASE_TOTAL_ELEMENTS;

    static QString textElementName;

#ifdef Q_OS_ANDROID
    AndroidScreenOn *screenOn;
#endif
};

#endif // RECEIVER_H
