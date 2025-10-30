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

#include <QMargins>


namespace AndroidEnvironment {
    int sdkVersion();
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
    static QString getPackageName();
    static QJniObject getSystemService(const QString &name);
    static QJniObject getContentResolver();
    static QJniObject getContext();
    static QJniObject getActivity();
    static QJniObject getWindow();
    static QJniObject getResources();
    static QJniObject getDecorView();
    static void runOnAndroidThread(const std::function<void()> &runnable);
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

class AndroidScreenOn : public AndroidUtilsBase
{
public:
    AndroidScreenOn();
    ~AndroidScreenOn();
private:
    QJniObject window;
};

/*============================================================*/

class AndroidContentReader : public AndroidUtilsBase
{
public:
    explicit AndroidContentReader(const QString &uri);
    explicit AndroidContentReader(const QJniObject &uri);
    ~AndroidContentReader();
    QString getFileName();
    QString getUri();
    qint64 getAvaiableBytes();
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
    explicit AndroidContentWriter(const QJniObject &uri);
    ~AndroidContentWriter();
    QString getFileName();
    QString getUri();
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

class AndroidStorage : AndroidUtilsBase
{
public:
    AndroidStorage() = default;

    static QJniObject parseUri(const QString &uriString);

    static bool hasUriPermission(const QString &uri, bool writable = true);
    static bool hasUriPermission(const QJniObject &uri, bool writable = true);
    static void grantUriPermission(const QJniObject &uri, bool writable = false);
    static void revokeUriPermission(const QJniObject &uri);

    static bool isDir(const QJniObject &uri);
    static bool exists(const QJniObject &parentDirUri, const QString &fileName, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    static QJniObject getEntry(const QJniObject &parentDirUri, const QString &childName, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    static QList<QJniObject> getEntryList(const QJniObject &dirUri);

    static QString getFileName(const QJniObject &uri);
    static qint64 getSize(const QJniObject &uri);
    static QString getMimeType(const QJniObject &uri);

    static QJniObject createDir(const QJniObject &parentDirUri, const QString &childDirName);
    static QJniObject createPath(const QJniObject &parentDirUri, const QStringList &path);
    static QJniObject createFile(const QJniObject &parentDirUri, const QString &fileName, const QString &mimeType = "*/*");
    static bool removeFile(const QJniObject &uri);

private:
    static QJniObject getDocumentUri(const QJniObject &uri);
    static QJniObject getChildDocumentsUri(const QJniObject &uri);
    static QString dirMimeType;
};

/*============================================================*/

class AndroidScreenArea : public AndroidUtilsBase
{
public:
    AndroidScreenArea() = default;
    static QMargins calcScreenSafeMargins();
    static void setSystemBarsMode(bool dark);
private:
    static Qt::ScreenOrientation getRotation();
    static int getResIdentifier(QJniObject &res, const QString &name);
    static int getStatusBarHeight();
    static int getNavBarHeight();
    static QMargins getSafeAreaMargins();
};

#endif

#endif // ANDROIDUTILS_H
