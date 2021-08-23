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
    AndroidUtilsBase() = default;
    static bool clearExceptions();
    static bool hasExceptions();

protected:
    static QJniObject getSystemService(const QString &name);
    static QJniObject getContentResolver();
    static QJniObject getContext();
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
    static QString getStringValue(Scope scope, const QString &key);
    static int32_t getIntValue(Scope scope, const QString &key);
private:
    static const char *className(Scope scope);
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
    explicit AndroidContentReader(const QJniObject &uri);
    ~AndroidContentReader();
    QString getFileName();
    qint64 getAvaiableBytes();
    bool open();
    QByteArray read(int size);
    int read(int size, char *buffer);
    void close();
private:
    QJniObject uriObject;
    QJniObject *stream = nullptr;
};

class AndroidContentWriter : public AndroidUtilsBase
{
public:
    explicit AndroidContentWriter(const QString &uri);
    explicit AndroidContentWriter(const QJniObject &uri);
    ~AndroidContentWriter();
    bool open();
    bool write(const QByteArray &data);
    bool write(const char *data, int size);
    void close();
private:
    QJniObject uriObject;
    QJniObject *stream = nullptr;
};

/*============================================================*/

class AndroidStorage : AndroidUtilsBase
{
public:
    AndroidStorage() = default;
    static bool requestPermission();
    static bool isPermissionGranted();

    static QString getExternalStorage();
    static QString convertToPath(const QString &url);

    static QJniObject parseUri(const QString &uriString);

    static bool isDir(const QJniObject &uri);
    static bool exists(const QJniObject &parentDirUri, const QString &fileName, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    static QJniObject getEntry(const QJniObject &parentDirUri, const QString &childName, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    static QList<QJniObject> getEntryList(const QJniObject &dirUri);

    static qint64 getSize(const QJniObject &uri);
    static QString getMimeType(const QJniObject &uri);

    static QJniObject createDir(const QJniObject &parentDirUri, const QString &childDirName);
    static QJniObject createPath(const QJniObject &parentDirUri, const QStringList &path);
    static QJniObject createFile(const QJniObject &parentDirUri, const QString &fileName, const QString &mimeType = "*/*");
    static bool removeFile(const QJniObject &uri);

private:
    static QJniObject getDocumentUri(const QJniObject &uri);
    static QJniObject getChildrenUri(const QJniObject &uri);
    static QString dirMimeType;
};


#endif

#endif // ANDROIDUTILS_H
