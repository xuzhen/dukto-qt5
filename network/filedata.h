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

#ifndef FILEDATA_H
#define FILEDATA_H

#include <QList>
#include <QStringList>
#include <QByteArray>

#ifdef Q_OS_ANDROID
#include "androidutils.h"
#else
class QFile;
#endif

class FileData
{
public:
    ~FileData();

    static QList<FileData> generateList(const QStringList &paths, qint64 &totalSize, QString &error);
    qint64 getSize() const;
    QString getName() const;
    QString getPath() const;
    bool isDir() const;

    void setName(const QString &newName);

    bool open();
    QByteArray read(qint64 size);
    bool eof();
    void close();

private:
    qint64 size;
    QString name;

#ifdef Q_OS_ANDROID
    FileData(qint64 size, const QString &relPath, const QJniObject &fullPath);
    static bool processDir(const QString &relPath, const QJniObject &fullUri, QList<FileData> &list, qint64 &totalSize, QString &error);
    AndroidContentReader *reader = nullptr;
    QJniObject path;
    qint64 readBytes = 0;
#else
    FileData(qint64 size, const QString &relPath, const QString &fullPath);
    static bool processDir(const QString &relPath, const QString &fullPath, QList<FileData> &list, qint64 &totalSize, QString &error);
    QFile *reader = nullptr;
    QString path;
#endif
};

#endif // FILEDATA_H
