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
 : size(size), name(relPath.toUtf8().append('\0')), path(fullPath) {
}

FileData::~FileData() {
    delete reader;
}

qint64 FileData::getSize() const {
    return size;
}

QByteArray FileData::getName() const {
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
    return reader->read(size);
}

bool FileData::eof() {
    if (reader == nullptr) {
        return true;
    }
#ifdef Q_OS_ANDROID
    return reader->getAvaiableBytes() == 0;
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
