#ifndef ANDROIDUTILS_H
#define ANDROIDUTILS_H

#include <QtGlobal>

#ifdef Q_OS_ANDROID

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QAndroidJniObject>
typedef QAndroidJniObject QJniObject;
#else
#include <QJniObject>
#endif


namespace AndroidEnvironment {
    int sdkVersion();
    int targetVersion();
    QString buildInfo(const QString &name);
}

/*============================================================*/

class AndroidUtilsBase
{
public:
    AndroidUtilsBase();
    static bool clearExceptions();
    static bool hasExceptions();

protected:
    QJniObject getSystemService(const QString &name);
    QJniObject getContentResolver();
    QJniObject getContext();
private:
    QJniObject context;
};

/*============================================================*/

class AndroidSettings : public AndroidUtilsBase
{
public:
    enum Scope {
        Global,
        Secure,
        System,
    };
    AndroidSettings() = default;
    QString getStringValue(Scope scope, const QString &key);
    int32_t getIntValue(Scope scope, const QString &key);
private:
    const char *className(Scope scope);
};

/*============================================================*/

class AndroidMulticastLock : public AndroidUtilsBase
{
public:
    AndroidMulticastLock();
    ~AndroidMulticastLock();
    bool acquire();
    void release();
private:
    QJniObject lock;
};

/*============================================================*/

class AndroidContentReader : public AndroidUtilsBase
{
public:
    explicit AndroidContentReader(const QString &uri);
    ~AndroidContentReader();
    QString getFileName();
    qint64 getSize();
    QString getMimeType();
    bool open();
    QByteArray read(int size);
    int read(int size, char *buffer);
    void close();
private:
    QString uriString;
    QJniObject uriObject;
    QJniObject *stream = nullptr;
};

class AndroidContentWriter : public AndroidUtilsBase
{
public:
    explicit AndroidContentWriter(const QString &uri);
    bool open();
    bool write(const QByteArray &data);
    bool write(const char *data, int size);
    void close();
private:
    QString uriString;
    QJniObject uriObject;
    QJniObject *stream = nullptr;
};

/*============================================================*/

class AndroidStorage : AndroidUtilsBase {
public:
    AndroidStorage() = default;
    static bool requestPermission();
    static bool isPermissionGranted();
    static QString getExternalStorage();
    static QString convertToPath(const QString &url);

    static QJniObject getUri(const QString &uriString);

    static bool isDir(const QString &uri);

    QString createDir(const QString &parentDirUri, const QString &subDirName);
    QString createFile(const QString &parentDirUri, const QString &fileName, const QString &mimeType = "*/*");
    bool removeFile(const QString &uri);

private:
    static bool isDir(const QJniObject &uriObject);
    QJniObject getDocumentUri(const QJniObject &uri);
};


#endif

#endif // ANDROIDUTILS_H
