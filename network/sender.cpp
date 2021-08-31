#include "sender.h"
#include <QTcpSocket>
#include <QTimer>

QByteArray Sender::textElementName = QStringLiteral("___DUKTO___TEXT___").toUtf8().append('\0');


Sender::Sender(const QString &dest, quint16 port, QObject *parent) : QObject(parent), socket(new QTcpSocket()), dest(dest), port(port) {
    connect(socket, &QTcpSocket::connected, this, &Sender::sendData, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::destroyed, this, &Sender::deleteLater, Qt::QueuedConnection);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(socket, &QTcpSocket::errorOccurred, this, &Sender::connectionError, Qt::QueuedConnection);
#else
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Sender::connectionError, Qt::QueuedConnection);
#endif
    connect(socket, &QTcpSocket::bytesWritten, this, &Sender::sendData);
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
        disconnectSlots();
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

                // all data sent
                if (waitBytesWritten) {
                    sendStatus = PHASE_FINALIZATION;
                    return;
                }
                // nothing written, go to PHASE_FINALIZATION directly
            }
            // fall through
            case PHASE_FINALIZATION: {
                filesToSend.clear();
                disconnectSlots();
                emit completed();
                QTimer::singleShot(1000, this, [this]() {
                    socket->disconnectFromHost();
                    socket->deleteLater();
                    socket = nullptr;
                });
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
    if (socket != nullptr) {
        emit aborted(error);
        abort();
    }
}

void Sender::disconnectSlots() {
    disconnect(socket, &QTcpSocket::connected, this, &Sender::sendData);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    disconnect(socket, &QTcpSocket::errorOccurred, this, &Sender::connectionError);
#else
    disconnect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Sender::connectionError);
#endif
    disconnect(socket, &QTcpSocket::bytesWritten, this, &Sender::sendData);
}
