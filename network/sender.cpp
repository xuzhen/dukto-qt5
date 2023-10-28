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

#include "sender.h"
#include <QTcpSocket>
#include <QTimer>

QByteArray Sender::textElementName = QStringLiteral("___DUKTO___TEXT___").toUtf8().append('\0');


Sender::Sender(const QString &dest, quint16 port, QObject *parent) : QObject(parent), socket(new QTcpSocket()), dest(dest), port(port) {
    connect(socket, &QTcpSocket::connected, this, &Sender::sendData);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(socket, &QTcpSocket::errorOccurred, this, &Sender::connectionError);
#else
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Sender::connectionError);
#endif
    connect(socket, &QTcpSocket::bytesWritten, this, &Sender::sendData);
}

Sender::~Sender() {
    abort();
}

void Sender::sendFiles(const QStringList &paths) {
    if (socket == nullptr) {
        return;
    }
    QString error;
    filesToSend = FileData::generateList(paths, totalBytes, error);
    if (error.isEmpty() == false) {
        reportError(error);
        return;
    }
    emit started(totalBytes);
    socket->connectToHost(dest, port, QTcpSocket::WriteOnly);
}

void Sender::sendFile(const QString &path, const QString &name) {
    if (socket == nullptr) {
        return;
    }
    QString error;
    filesToSend = FileData::generateList(QStringList() << path, totalBytes, error);
    if (error.isEmpty() == false) {
        reportError(error);
        return;
    }
    if (name.isEmpty() == false) {
        filesToSend[0].setName(name);
    }
    emit started(totalBytes);
    socket->connectToHost(dest, port, QTcpSocket::WriteOnly);
}


void Sender::sendText(const QString &text) {
    if (socket == nullptr) {
        return;
    }
    filesToSend.clear();
    textToSend = text.toUtf8();
    totalBytes = textToSend.size();
    emit started(totalBytes);
    socket->connectToHost(dest, port, QTcpSocket::WriteOnly);
}

void Sender::abort() {
    if (socket != nullptr) {
        socket->disconnect(this);
        socket->abort();
        socket->deleteLater();
        socket = nullptr;
    }
}

void Sender::sendData() {
    while (socket != nullptr) {
        switch (sendStatus) {
            case PHASE_TOTAL_ELEMENTS_AND_SIZE: {
                qint64 totalElements;
                if (filesToSend.size() == 0) {
                    // text
                    totalElements = 1;
                } else {
                    // file / directory
                    totalElements = filesToSend.size();
                }
                QByteArray bytes(reinterpret_cast<char *>(&totalElements), sizeof(totalElements));
                bytes.append(reinterpret_cast<char *>(&totalBytes), sizeof(totalBytes));
                socket->write(bytes);
                currentFileIndex = 0;
                sendStatus = PHASE_ELEMENT_NAME_AND_SIZE;
                return;
            }
            case PHASE_ELEMENT_NAME_AND_SIZE: {
                QByteArray fileName;
                qint64 size;
                if (filesToSend.size() == 0) {
                    // text
                    fileName = textElementName;
                    size = textToSend.size();
                } else {
                    // file / directory
                    currentFile = &(filesToSend[currentFileIndex]);
                    fileName = currentFile->getName();
                    size = currentFile->getSize();
                    if (currentFile->isDir() == false) {
                        if (currentFile->open() == false) {
                            reportError(QStringLiteral("Can not read %1").arg(currentFile->getPath()));
                            return;
                        }
                    }
                }

                QByteArray bytes = fileName;
                bytes.append(reinterpret_cast<char *>(&size), sizeof(size));
                socket->write(bytes);
                sendStatus = PHASE_ELEMENT_DATA;
                return;
            }
            case PHASE_ELEMENT_DATA: {
                if (socket->bytesToWrite() >= 1024 * 1024) {
                    // do not leave too much data in sending buffer
                    return;
                }
                bool waitBytesWritten = false;
                if (filesToSend.size() == 0) {
                    // text
                    socket->write(textToSend);
                    totalBytesSent += textToSend.size();
                    waitBytesWritten = true;
                } else if (currentFile->isDir() == false) {
                    // file
                    if (currentFile->getSize() > 0)  {
                        QByteArray d = currentFile->read(1024 * 1024);
                        if (d.size() > 0) {
                            socket->write(d);
                            totalBytesSent += d.size();
                            waitBytesWritten = true;
                        }
                    }
                } else {
                     // no data for directory
                }

                emit progress(totalBytes, totalBytesSent);

                if (filesToSend.size() == 0 || currentFile->isDir()) {
                    // text or directory
                    currentFileIndex++;
                    textToSend.clear();
                    sendStatus = PHASE_ELEMENT_NAME_AND_SIZE;
                } else if (currentFile->eof()) {
                    // whole file sent
                    currentFileIndex++;
                    sendStatus = PHASE_ELEMENT_NAME_AND_SIZE;
                    currentFile->close();
                    currentFile = nullptr;
                }

                if (currentFileIndex < filesToSend.size()) {
                    if (waitBytesWritten) {
                        return;
                    } else {
                        break;
                    }
                }

                // all files written
                sendStatus = PHASE_FINALIZATION;
                if (waitBytesWritten) {
                    return;
                }
                // nothing written, go to PHASE_FINALIZATION directly
            }
            // fall through
            case PHASE_FINALIZATION: {
                if (socket->bytesToWrite() == 0) {
                    // end connection until all data sent
                    filesToSend.clear();
                    socket->disconnect(this);
                    socket->disconnectFromHost();
                    socket->deleteLater();
                    socket = nullptr;
                    emit completed();
                }
                return;
            }
        }
    }

}

void Sender::connectionError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    reportError(socket->errorString());
}


void Sender::reportError(const QString &error) {
    emit aborted(error);
    abort();
}
