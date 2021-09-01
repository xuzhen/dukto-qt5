#include "receiver.h"
#include <QHostAddress>
#include <algorithm>

#ifdef Q_OS_ANDROID
#include "androidutils.h"
#else
#include <QDir>
#include <QFile>
#endif

QString Receiver::textElementName = QStringLiteral("___DUKTO___TEXT___");

Receiver::Receiver(QTcpSocket *socket, const QString &destDir, QObject *parent) : QObject(parent), socket(socket), destDir(destDir) {
    connect(socket, &QTcpSocket::readyRead, this, &Receiver::processData);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(socket, &QTcpSocket::errorOccurred, this, &Receiver::connectionError);
#else
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Receiver::connectionError);
#endif

#ifdef Q_OS_ANDROID
    screenOn = new AndroidScreenOn();
#endif

    if (socket->bytesAvailable()) {
        processData();
    }
}


Receiver::~Receiver() {
    if (socket != nullptr) {
        socket->disconnect(this);
        socket->deleteLater();
    }
    delete currentFile;
#ifdef Q_OS_ANDROID
    delete screenOn;
#endif
}

void Receiver::processData() {
    /*
     * total element count
     * total element size
     * first element {
     *   element name
     *   element size
     *   element data
     * }
     * second element {
     *   ...
     * }
     * ...
     */
    while (socket->bytesAvailable() > 0) {
        switch (recvStatus) {
            case PHASE_TOTAL_ELEMENTS: {
                if (socket->bytesAvailable() < static_cast<qint64>(sizeof(sessionElements))) {
                    // wait for more data
                    return;
                }
                socket->read(reinterpret_cast<char*>(&sessionElements), sizeof(sessionElements));
                if (sessionElements <= 0) {
                    // invalid data
                    terminateConnection();
                    return;
                }
                recvStatus = PHASE_TOTAL_SIZE;
                break;
            }
            case PHASE_TOTAL_SIZE: {
                if (socket->bytesAvailable() < static_cast<qint64>(sizeof(sessionBytes))) {
                    // wait for more data
                    return;
                }
                socket->read(reinterpret_cast<char*>(&sessionBytes), sizeof(sessionBytes));
                if (sessionBytes < 0) {
                    // invalid data
                    terminateConnection();
                    return;
                }
                recvStatus = PHASE_ELEMENT_NAME;
                emit started(sessionBytes);
                break;
            }
            case PHASE_ELEMENT_NAME: {
                char c;
                while (true) {
                    if (socket->read(&c, sizeof(c)) < static_cast<qint64>(sizeof(c))) {
                        // wait for more data
                        return;
                    }
                    readBuffer.append(c);
                    if (c == '\0') {
                        currentElementName = QString::fromUtf8(readBuffer);
                        readBuffer.clear();
                        if (currentElementName.isEmpty()) {
                            // invalid data;
                            terminateSession(QStringLiteral("received invalid data from %1").arg(socket->peerAddress().toString()));
                            return;
                        }
                        recvStatus = PHASE_ELEMENT_SIZE;
                        break;
                    }
                }
                break;
            }
            case PHASE_ELEMENT_SIZE: {
                if (socket->bytesAvailable() < static_cast<qint64>(sizeof(currentElementBytes))) {
                    // wait for more data
                    return;
                }
                socket->read((char*) &currentElementBytes, sizeof(qint64));
                if (currentElementBytes < -1) {
                    // invalid data;
                    terminateSession(QStringLiteral("received invalid data from %1").arg(socket->peerAddress().toString()));
                    return;
                }

                if (currentElementName == textElementName) {
                    // text
                    currentElementType = TEXT_ELEMENT;
                    currentElementReceived = 0;
                    recvStatus = PHASE_ELEMENT_DATA;
                } else if (currentElementBytes == -1) {
                    // directory
                    currentElementType = DIR_ELEMENT;
                    if (prepareFilesystem() == false) {
                        return;
                    }
                    if (currentElementName.contains(QChar('/')) == false) {
                        emit dirReceived(currentTopElementName, currentTopElementPath);
                    }
                    sessionElementsReceived++;
                    if (sessionElementsReceived < sessionElements) {
                        recvStatus = PHASE_ELEMENT_NAME;
                        break;
                    } else {
                        endSession();
                        return;
                    }
                } else {
                    // file
                    currentElementType = FILE_ELEMENT;
                    if (prepareFilesystem() == false) {
                        return;
                    }
                    currentElementReceived = 0;
                    recvStatus = PHASE_ELEMENT_DATA;
                }
                if (currentElementBytes > 0) {
                    break;
                }
                // an empty file, go to PHASE_ELEMENT_DATA
            }
            // fall through
            case PHASE_ELEMENT_DATA: {
                if (currentElementBytes > 0) {
                    QByteArray d = socket->read(std::min<qint64>(currentElementBytes - currentElementReceived, 1024 * 1024));
                    currentElementReceived += d.size();
                    sessionBytesReceived += d.size();
                    emit progress(sessionBytes, sessionBytesReceived);

                    if (currentElementType == TEXT_ELEMENT) {
                        readBuffer.append(d);
                    } else {
#ifdef Q_OS_ANDROID
                        if (currentFile->write(d) == false) {
#else
                        if (currentFile->write(d) < d.size()) {
#endif
                            terminateSession(QStringLiteral("Failed to write to %1").arg(currentElementName));
                            return;
                        }
                    }
                }

                if (currentElementReceived == currentElementBytes) {
                    // received all bytes
                    sessionElementsReceived++;
                    if (currentElementType == TEXT_ELEMENT) {
                        // text
                        emit textReceived(QString::fromUtf8(readBuffer));
                        readBuffer.clear();
                    } else {
                        // file
                        if (currentElementName.contains(QChar('/')) == false) {
                            emit fileReceived(currentTopElementName, currentTopElementPath, currentElementBytes);
                        }
                        currentFile->close();
                        delete currentFile;
                        currentFile = nullptr;
                    }
                    if (sessionElementsReceived < sessionElements) {
                        recvStatus = PHASE_ELEMENT_NAME;
                    } else {
                        endSession();
                        return;
                    }
                }
                break;
            }
        }
    }
}

void Receiver::endSession() {
    emit completed();
    terminateConnection();
}


void Receiver::terminateSession(const QString &error) {
    emit aborted(error);
    terminateConnection();
}

void Receiver::terminateConnection() {
    if (socket != nullptr) {
        socket->disconnect(this);
        socket->close();
        socket->deleteLater();
        socket = nullptr;
    }
}

bool Receiver::prepareFilesystem() {
#ifdef Q_OS_ANDROID
    QStringList dirs = currentElementName.split(QChar('/'));
    if (currentElementType == DIR_ELEMENT) {
        // the element is a directory
        dirs[0] = getNewPath(dirs.at(0));
        QJniObject uri = AndroidStorage::createPath(AndroidStorage::parseUri(destDir), dirs);
        if (uri.isValid() == false) {
            terminateSession(QStringLiteral("Failed to create directory %1").arg(currentElementName));
            return false;
        }
        if (dirs.length() == 1) {
            currentTopElementName = dirs.at(0);
            currentTopElementPath = uri.toString();
        }
    } else {
        // the element is a file
        QString fileName = dirs.takeLast();
        // prepare parent directories
        QJniObject parentDirUri;
        if (dirs.length() == 0) {
            // no parent directories
            fileName = getNewFileName(destDir, fileName);
            parentDirUri = AndroidStorage::parseUri(destDir);
        } else {
            dirs[0] = getNewPath(dirs.at(0));
            parentDirUri = AndroidStorage::createPath(AndroidStorage::parseUri(destDir), dirs);
            if (parentDirUri.isValid() == false) {
                terminateSession(QStringLiteral("Failed to create directory %1").arg(dirs.join(QChar('/'))));
                return false;
            }
        }
        // create an empty file
        QJniObject fileUri = AndroidStorage::createFile(parentDirUri, fileName);
        if (fileUri.isValid() == false) {
            terminateSession(QStringLiteral("Failed to create file %1").arg(currentElementName));
            return false;
        }
        // create a writer object for the file
        currentFile = new AndroidContentWriter(fileUri);
        if (!currentFile->open()) {
            AndroidStorage::removeFile(fileUri);
            delete currentFile;
            currentFile = nullptr;
            terminateSession(QStringLiteral("Can not write to %1").arg(currentElementName));
            return false;
        }
        if (dirs.length() == 0) {
            currentTopElementName = currentFile->getFileName();
            currentTopElementPath = fileUri.toString();
        }
    }
#else
    if (currentElementType == DIR_ELEMENT) {
        // a directory
        QString dirPath = getNewPath(currentElementName);
        // create all parent directories
        QDir d(destDir);
        QString absPath = d.filePath(dirPath);
        if (d.mkpath(absPath) == false) {
            terminateSession(QStringLiteral("Failed to create directory %1").arg(absPath));
            return false;
        }
        if (currentElementName.contains(QChar('/')) == false) {
            currentTopElementName = dirPath;
            currentTopElementPath = absPath;
        }
    } else {
        // a file
        QString filePath;
        int index = currentElementName.lastIndexOf(QChar('/'));
        if (index >= 0) {
            QString dirPath = getNewPath(currentElementName.left(index));
            filePath = dirPath + currentElementName.mid(index);
            QDir d(destDir);
            dirPath = d.filePath(dirPath);
            if (d.mkpath(dirPath) == false) {
                terminateSession(QStringLiteral("Failed to create directory %1").arg(dirPath));
                return false;
            }
        } else {
            // with no parent directories
            filePath = getNewFileName(destDir, currentElementName);
            currentTopElementName = filePath;
        }
        filePath = QDir(destDir).filePath(filePath);
        currentFile = new QFile(filePath);
        if (currentFile->open(QFile::WriteOnly) == false) {
            delete currentFile;
            currentFile = nullptr;
            terminateSession(QStringLiteral("Can not write to %1").arg(filePath));
            return false;
        }
        if (index < 0) {
            currentTopElementPath = filePath;
        }
    }
#endif
    return true;
}

QString Receiver::getNewPath(const QString &originalPath) {
    QString rootDir = originalPath.section(QChar('/'), 0);
    if (dirNameMap.contains(rootDir)) {
        QString newRootDir = dirNameMap.value(rootDir);
        if (newRootDir.isEmpty() == false) {
            return QString(originalPath).replace(0, rootDir.size(), newRootDir);
        } else {
            return originalPath;
        }
    } else {
        QString newRootDir = getNewFileName(destDir, rootDir);
        if (newRootDir != rootDir) {
            dirNameMap.insert(rootDir, newRootDir);
            return QString(originalPath).replace(0, rootDir.size(), newRootDir);
        } else {
            dirNameMap.insert(rootDir, QString());
            return originalPath;
        }
    }
}

QString Receiver::getNewFileName(const QString &parentDir, const QString &originalName) {
    int i = 2;
    QString newName = originalName;
    QString baseName = originalName.section(QChar('.'), 0, 0);
    QString suffix = originalName.section(QChar('.'), 1);
    if (suffix.isEmpty() == false) {
        suffix.prepend(QChar('.'));
    }
#ifdef Q_OS_ANDROID
    QJniObject parentUri = AndroidStorage::parseUri(parentDir);
    while (AndroidStorage::exists(parentUri, newName)) {
        newName = baseName + " (" + QString::number(i++) + ")" + suffix;
    }
#else
    while (QFile::exists(QDir(parentDir).filePath(newName))) {
        newName = baseName + " (" + QString::number(i++) + ")" + suffix;
    }
#endif
    return newName;
}

void Receiver::connectionError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    if (socket != nullptr) {
        emit aborted(socket->errorString());
        terminateConnection();
    }
}
