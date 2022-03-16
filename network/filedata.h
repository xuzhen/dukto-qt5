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
    QByteArray getName() const;
    QString getPath() const;
    bool isDir() const;

    void setName(const QString &newName);

    bool open();
    QByteArray read(qint64 size);
    bool eof();
    void close();

private:
    qint64 size;
    QByteArray name;

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
