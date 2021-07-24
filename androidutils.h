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

protected:
    QJniObject getSystemService(const QString &name);
    QJniObject getContentResolver();
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
    bool isDir();
    bool open();
    QByteArray read(int size);
    int read(int size, char *buffer);
    void close();
private:
    QString uriString;
    QJniObject uriObject;
    QJniObject *stream = nullptr;
};

/*============================================================*/
namespace AndroidStorage {
    bool requestPermission();
    bool isPermissionGranted();
    QString getExternalStorage();
    QString convertToPath(const QString &url);
}


#endif

#endif // ANDROIDUTILS_H
