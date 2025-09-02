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

#include "filedata.h"

#ifndef Q_OS_ANDROID
#include <QFile>
#include <QFileInfo>
#include <QDir>
#endif

#ifdef Q_OS_ANDROID
FileData::FileData(qint64 size, const QString &relPath, const QJniObject &fullPath)
#else
FileData::FileData(qint64 size, const QString &relPath, const QString &fullPath)
#endif
 : size(size), name(relPath), path(fullPath) {
}

FileData::~FileData() {
    delete reader;
}

qint64 FileData::getSize() const {
    return size;
}

QString FileData::getName() const {
    return name;
}

QString FileData::getPath() const {
#ifdef Q_OS_ANDROID
    return path.toString();
#else
    return path;
#endif
}

bool FileData::isDir() const {
    return size == -1;
}

void FileData::setName(const QString &newName) {
    if (newName.isEmpty() == false) {
        name = newName.toUtf8().append('\0');
    }
}

bool FileData::open() {
#ifdef Q_OS_ANDROID
    if (reader == nullptr) {
        reader = new AndroidContentReader(path);
    }
    readBytes = 0;
    return reader->open();
#else
    if (reader == nullptr) {
        reader = new QFile(path);
    }
    return reader->open(QFile::ReadOnly);
#endif
}

QByteArray FileData::read(qint64 size) {
    if (reader == nullptr)  {
        return QByteArray();
    }
#ifdef Q_OS_ANDROID
    QByteArray d = reader->read(size);
    readBytes += d.size();
    return d;
#else
    return reader->read(size);
#endif
}

bool FileData::eof() {
    if (reader == nullptr) {
        return true;
    }
#ifdef Q_OS_ANDROID
    return readBytes >= size;
#else
    return reader->atEnd();
#endif
}

void FileData::close() {
    if (reader != nullptr) {
        reader->close();
        delete reader;
        reader = nullptr;
    }
}

QList<FileData> FileData::generateList(const QStringList &paths, qint64 &totalSize, QString &error) {
    QList<FileData> list;
    totalSize = 0;
    error.clear();
    for (const QString &path: paths) {
#ifdef Q_OS_ANDROID
        QJniObject uri = AndroidStorage::parseUri(path);
        AndroidContentReader reader(uri);
        if (processDir(reader.getFileName(), uri, list, totalSize, error) == false) {
            return QList<FileData>();
        }
#else
        QString cleanPath = QDir::cleanPath(path);
        if (cleanPath.endsWith(QChar('/'))) {
            cleanPath.chop(1);
        }
        if (processDir(QFileInfo(cleanPath).fileName(), cleanPath, list, totalSize, error) == false) {
            return QList<FileData>();
        }
#endif
    }
    return list;
}

#ifdef Q_OS_ANDROID

bool FileData::processDir(const QString &relPath, const QJniObject &fullUri, QList<FileData> &list, qint64 &totalSize, QString &error) {
    if (AndroidStorage::isDir(fullUri)) {
        list.append(FileData(-1, relPath, fullUri));
        const QList<QJniObject> uris = AndroidStorage::getEntryList(fullUri);
        for (const QJniObject &uri : uris) {
            QString name = AndroidContentReader(uri).getFileName();
            if (processDir(relPath + "/" + name, uri, list, totalSize, error) == false) {
                return false;
            }
        }
    } else {
        qint64 size = AndroidStorage::getSize(fullUri);
        if (size == -1) {
            error = QStringLiteral("Can not read file %1").arg(fullUri.toString());
            return false;
        }
        AndroidContentReader reader(fullUri);
        if (reader.open() == false) {
            error = QStringLiteral("Can not read file %1").arg(fullUri.toString());
            return false;
        }
        reader.close();

        list.append(FileData(size, relPath, fullUri));
        totalSize += size;
    }
    return true;
}

#else

bool FileData::processDir(const QString &relPath, const QString &fullPath, QList<FileData> &list, qint64 &totalSize, QString &error) {
    QFileInfo info(fullPath);
    if (info.isReadable() == false) {
        error = QStringLiteral("Can not read %1").arg(fullPath);
        return false;
    }
    if (info.isDir()) {
        list.append(FileData(-1, relPath, fullPath));
        const QStringList entries = QDir(fullPath).entryList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
        for (const QString &entry : entries) {
            if (processDir(relPath + "/" + entry, fullPath + "/" + entry, list, totalSize, error) == false) {
                return false;
            }
        }
    } else {
        list.append(FileData(info.size(), relPath, fullPath));
        totalSize += info.size();
    }
    return true;
}

#endif
