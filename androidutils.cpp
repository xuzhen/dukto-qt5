#include "androidutils.h"

#ifdef Q_OS_ANDROID

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtAndroid>
#include <QAndroidIntent>
#include <QAndroidJniEnvironment>
typedef QAndroidJniEnvironment QJniEnvironment;
#else
#include <QCoreApplication>
#include <QJniEnvironment>
#include <QFutureWatcher>
#endif

#include <QUrl>
#include <QDir>

int AndroidEnvironment::sdkVersion() {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    static int sdkVer = QtAndroid::androidSdkVersion();
#else
    static int sdkVer = QNativeInterface::QAndroidApplication::sdkVersion();
#endif
    return sdkVer;
}

int AndroidEnvironment::targetVersion() {
    return ANDROID_TARGET_SDK_VERSION;
}

QString AndroidEnvironment::buildInfo(const QString &name) {
    return QJniObject::getStaticObjectField("android/os/Build", name.toLatin1().constData(), "Ljava/lang/String;").toString();
}

/*============================================================*/

/*
QString AndroidUtilsBase::getPackageName() {
    static QString packageName;
    if (packageName.isEmpty()) {
        packageName = getContext().callObjectMethod("getPackageName", "()Ljava/lang/String;").toString();
    }
    return packageName;
}
*/

QJniObject AndroidUtilsBase::getSystemService(const QString &name) {
    if (getContext().isValid()) {
        QJniObject serviceString = QJniObject::fromString(name);
        QJniObject service = getContext().callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;", serviceString.object<jstring>());
        clearExceptions();
        return service;
    } else {
        return QJniObject();
    }
}

QJniObject AndroidUtilsBase::getContentResolver() {
    static QJniObject resolver;
    if (resolver.isValid() == false) {
        if (getContext().isValid()) {
            resolver = getContext().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
            clearExceptions();
        }
    }
    return resolver;
}

QJniObject AndroidUtilsBase::getContext() {
    static QJniObject context;
    if (context.isValid() == false) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        context = QtAndroid::androidActivity().callObjectMethod("getApplicationContext","()Landroid/content/Context;");
#else
        context = qApp->nativeInterface<QNativeInterface::QAndroidApplication>()->context();
#endif
    }
    return context;
}

bool AndroidUtilsBase::clearExceptions() {
    static QJniEnvironment env;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return true;
    }
    return false;
#else
    return env.checkAndClearExceptions();
#endif
}

bool AndroidUtilsBase::hasExceptions() {
    static QJniEnvironment env;
    return env->ExceptionCheck();
}

/*============================================================*/

const char *AndroidSettings::className(Scope scope) {
    switch (scope) {
    case Global:
        return "android/provider/Settings$Global";
    case Secure:
        return "android/provider/Settings$Secure";
    case System:
        return "android/provider/Settings$System";
    }
}

QString AndroidSettings::getStringValue(Scope scope, const QString &key) {
    return QJniObject::callStaticObjectMethod(className(scope),
                                              "getString",
                                              "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;",
                                              getContentResolver().object(),
                                              QJniObject::fromString(key).object<jstring>()).toString();
}

int32_t AndroidSettings::getIntValue(Scope scope, const QString &key) {
    return QJniObject::callStaticMethod<jint>(className(scope),
                                              "getInt",
                                              "(Landroid/content/ContentResolver;Ljava/lang/String;)I",
                                              getContentResolver().object(),
                                              QJniObject::fromString(key).object<jstring>());
}

/*============================================================*/

AndroidMulticastLock::AndroidMulticastLock() {
    QJniObject wifiManager = getSystemService("wifi");
    if (wifiManager.isValid()) {
        lock = wifiManager.callObjectMethod("createMulticastLock", "(Ljava/lang/String;)Landroid/net/wifi/WifiManager$MulticastLock;", QJniObject::fromString("Dukto").object<jstring>());
    }
}

AndroidMulticastLock::~AndroidMulticastLock() {
    release();
}

bool AndroidMulticastLock::acquire() {
    if (lock.isValid() == false) {
        return false;
    }
    if (lock.callMethod<jboolean>("isHeld")) {
        return true;
    }
    lock.callMethod<void>("acquire");
    return lock.callMethod<jboolean>("isHeld");
}

void AndroidMulticastLock::release() {
    if (lock.isValid() == false || !lock.callMethod<jboolean>("isHeld")) {
        return;
    }
    lock.callMethod<void>("release");
}

/*============================================================*/


AndroidContentReader::AndroidContentReader(const QString &uri) : uriObject(AndroidStorage::getUri(uri)) {
}

AndroidContentReader::AndroidContentReader(const QJniObject &uri) : uriObject(uri) {
}

AndroidContentReader::~AndroidContentReader() {
    close();
}

QString AndroidContentReader::getFileName() {
    if (uriObject.isValid() == false) {
        return "";
    }
    QString filename;
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", uriObject.object<jstring>(), nullptr, nullptr, nullptr, nullptr);
    if (cursor.isValid()) {
        if (cursor.callMethod<jboolean>("moveToFirst")) {
            // getColumnIndex(OpenableColumns.DISPLAY_NAME)
            jint index = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", QJniObject::fromString("_display_name").object<jstring>());
            if (index != -1) {
                filename = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", index).toString();
            }
        }
        cursor.callMethod<void>("close");
    }
    clearExceptions();
    if (filename.isEmpty()) {
        filename = uriObject.callObjectMethod("getPath", "()Ljava/lang/String;").toString();
        clearExceptions();
        int pos = filename.lastIndexOf('/');
        if (pos >= 0) {
            filename = filename.mid(pos + 1);
        }
    }
    return filename;
}

qint64 AndroidContentReader::getSize() {
    qint64 size = -1;
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", uriObject.object<jstring>(), nullptr, nullptr, nullptr, nullptr);
    if (cursor.isValid()) {
        if (cursor.callMethod<jboolean>("moveToFirst")) {
            // getColumnIndex(OpenableColumns.SIZE)
            jint index = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", QJniObject::fromString("_size").object<jstring>());
            if (index != -1) {
                size = cursor.callMethod<jlong>("getLong", "(I)J", index);
            }
        }
        cursor.callMethod<void>("close");
    }
    clearExceptions();
    if (size != -1) {
        return size;
    }

    if (stream == nullptr) {
        return -1;
    }
    size = stream->callMethod<jint>("available", "()I");
    return size;
}

QString AndroidContentReader::getMimeType() {
    if (uriObject.isValid() == false) {
        return "";
    }
    QString mimeType = getContentResolver().callObjectMethod("getType", "(Landroid/net/Uri;)Ljava/lang/String;", uriObject.object<jstring>()).toString();
    clearExceptions();
    return mimeType;
}

bool AndroidContentReader::open() {
    close();
    if (uriObject.isValid() == false) {
        return false;
    }
    stream = new QJniObject(getContentResolver().callObjectMethod("openInputStream", "(Landroid/net/Uri;)Ljava/io/InputStream;", uriObject.object()));
    if (stream->isValid() == false) {
        clearExceptions();
        delete stream;
        stream = nullptr;
        return false;
    }
    return true;
}

QByteArray AndroidContentReader::read(int size) {
    QByteArray buffer;
    buffer.resize(size);
    read(size, buffer.data());
    return buffer;
}

int AndroidContentReader::read(int size, char* buffer) {
    if (stream == nullptr) {
        return -1;
    }
    QJniEnvironment env;
    jbyteArray array = env->NewByteArray(size);
    if (array == nullptr) {
        return -1;
    }
    jint r = stream->callMethod<jint>("read", "([B)I", array);
    if (r > 0) {
        env->GetByteArrayRegion(array, 0, r, reinterpret_cast<jbyte*>(buffer));
    }
    env->DeleteLocalRef(array);
    clearExceptions();
    return r;
}

void AndroidContentReader::close() {
    if (stream == nullptr) {
        return;
    }
    stream->callMethod<void>("close");
    clearExceptions();
    delete stream;
    stream = nullptr;
}

/*============================================================*/

AndroidContentWriter::AndroidContentWriter(const QString &uri) : uriObject(AndroidStorage::getUri(uri)) {
}

AndroidContentWriter::AndroidContentWriter(const QJniObject &uri) : uriObject(uri) {
}

bool AndroidContentWriter::open() {
    if (uriObject.isValid() == false) {
        return false;
    }
    close();
    stream = new QJniObject(getContentResolver().callObjectMethod("openOutputStream", "(Landroid/net/Uri;)Ljava/io/OutputStream;", uriObject.object()));
    if (stream->isValid() == false) {
        clearExceptions();
        delete stream;
        stream = nullptr;
        return false;
    }
    return true;
}

bool AndroidContentWriter::write(const QByteArray &data) {
    return write(data.constData(), data.size());
}

bool AndroidContentWriter::write(const char *data, int size) {
    if (stream == nullptr) {
        return false;
    }
    QJniEnvironment env;
    jbyteArray array = env->NewByteArray(size);
    if (array == nullptr) {
        return false;
    }
    env->SetByteArrayRegion(array, 0, size, reinterpret_cast<const jbyte*>(data));
    stream->callMethod<void>("write", "([B)V", array);
    bool r = hasExceptions();
    env->DeleteLocalRef(array);
    clearExceptions();
    return r;
}

void AndroidContentWriter::close() {
    if (stream == nullptr) {
        return;
    }
    stream->callMethod<void>("close");
    clearExceptions();
    delete stream;
    stream = nullptr;
}


/*============================================================*/

bool AndroidStorage::requestPermission() {
    if (AndroidEnvironment::sdkVersion() < 23) {
        return true;
    }
    QStringList permissions;
    permissions << "android.permission.WRITE_EXTERNAL_STORAGE";
    if (AndroidEnvironment::targetVersion() >= 30) {
        permissions << "android.permission.MANAGE_EXTERNAL_STORAGE";
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const QtAndroid::PermissionResultMap result = QtAndroid::requestPermissionsSync(permissions);
    for (const QtAndroid::PermissionResult &r: result) {
        if (r != QtAndroid::PermissionResult::Granted) {
            return false;
        }
    }
    return true;
#else
    QFutureWatcher<QPermission::PermissionResult> watcher;
    for (const QString &p: permissions) {
        QFuture<QPermission::PermissionResult> future = qApp->requestPermission(p);
        watcher.setFuture(future);
        watcher.waitForFinished();
        if (future.result() != QPermission::Authorized) {
            return false;
        }
    }
    return true;
#endif
}

bool AndroidStorage::isPermissionGranted() {
    QStringList permissions;
    permissions << "android.permission.WRITE_EXTERNAL_STORAGE";
    if (AndroidEnvironment::targetVersion() >= 30) {
        permissions << "android.permission.MANAGE_EXTERNAL_STORAGE";
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    for (const QString &p: permissions) {
        if (QtAndroid::checkPermission(p) != QtAndroid::PermissionResult::Granted) {
            return false;
        }
    }
    return true;
#else
    QFutureWatcher<QPermission::PermissionResult> watcher;
    for (const QString &p: permissions) {
        QFuture<QPermission::PermissionResult> future = qApp->checkPermission(p);
        watcher.setFuture(future);
        watcher.waitForFinished();
        if (future.result() != QPermission::Authorized) {
            return false;
        }
    }
    return true;
#endif
}

QString AndroidStorage::getExternalStorage() {
    static QString storagePath;
    if (storagePath.isEmpty()) {
        QJniObject mediaDir = QJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
        QJniObject mediaPath = mediaDir.callObjectMethod("getAbsolutePath", "()Ljava/lang/String;");
        storagePath = mediaPath.toString();
    }
    return storagePath;
}

QString AndroidStorage::convertToPath(const QString &url) {
    static const QStringList contentUrls = QStringList() <<
                                           QStringLiteral("content://com.android.externalstorage.documents/tree/primary%3A") <<
                                           QStringLiteral("content://com.android.externalstorage.documents/document/primary%3A");
    for (const QString &u: contentUrls) {
        if (url.startsWith(u)) {
            return QDir(getExternalStorage()).filePath(QUrl::fromPercentEncoding(url.mid(u.length()).toUtf8()));
        }
    }
    return url;
}

QJniObject AndroidStorage::getUri(const QString &uriString) {
    QJniObject uri = QJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", QJniObject::fromString(uriString).object<jstring>());
    if (uri.isValid() == false) {
        clearExceptions();
    }
    return uri;
}

bool AndroidStorage::isDir(const QString &uri) {
    return isDir(getUri(uri));
}

bool AndroidStorage::isDir(const QJniObject &uri) {
    if (uri.isValid() == false) {
        return false;
    }
    jboolean r = QJniObject::callStaticMethod<jboolean>("android/provider/DocumentsContract", "isTreeUri", "(Landroid/net/Uri;)Z", uri.object());
    clearExceptions();
    return r;
}

QJniObject AndroidStorage::getDocumentUri(const QJniObject &uri) {
    if (QJniObject::callStaticMethod<jboolean>("android/provider/DocumentsContract", "isDocumentUri", "(Landroid/content/Context;Landroid/net/Uri;)Z", getContext().object(), uri.object())) {
        return uri;
    }
    QJniObject docId = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "getTreeDocumentId", "(Landroid/net/Uri;)Ljava/lang/String;", uri.object());
    if (!docId.isValid()) {
        clearExceptions();
        return QJniObject();
    }
    QJniObject docUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "buildDocumentUriUsingTree", "(Landroid/net/Uri;Ljava/lang/String;)Landroid/net/Uri;", uri.object(), docId.object());
    if (!docUri.isValid()) {
        clearExceptions();
        return QJniObject();
    }
    return docUri;
}

QList<QJniObject> AndroidStorage::getEntryList(const QString &dirUri) {
    return getEntryList(getUri(dirUri));
}

QList<QJniObject> AndroidStorage::getEntryList(const QJniObject &dirUri) {
    if (isDir(dirUri) == false) {
        return QList<QJniObject>();
    }
    QJniObject dirDocUri = getDocumentUri(dirUri);
    if (dirDocUri.isValid() == false) {
        return QList<QJniObject>();
    }
    QJniObject dirDocId = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "getDocumentId", "(Landroid/net/Uri;)Ljava/lang/String;", dirDocUri.object());
    if (dirDocId.isValid() == false) {
        clearExceptions();
        return QList<QJniObject>();
    }
    QJniObject childrenUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "buildChildDocumentsUriUsingTree", "(Landroid/net/Uri;Ljava/lang/String;)Landroid/net/Uri;", dirDocUri.object(), dirDocId.object());
    if (childrenUri.isValid() == false) {
        clearExceptions();
        return QList<QJniObject>();
    }
    QList<QJniObject> entries;
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", childrenUri.object<jstring>(), nullptr, nullptr, nullptr, nullptr);
    if (cursor.isValid()) {
        jint idIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", QJniObject::fromString("document_id").object<jstring>());
        if (idIndex != -1) {
            while (cursor.callMethod<jboolean>("moveToNext")) {
                QJniObject childDocId = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", idIndex);
                QJniObject childDocUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "buildDocumentUriUsingTree", "(Landroid/net/Uri;Ljava/lang/String;)Landroid/net/Uri;", childrenUri.object(), childDocId.object());
                if (childDocUri.isValid() == false) {
                    clearExceptions();
                    return QList<QJniObject>();
                }
                entries.append(childDocUri);
            }
        }
        cursor.callMethod<void>("close");
    }
    clearExceptions();
    return entries;
}

QString AndroidStorage::createDir(const QString &parentDirUri, const QString &subDirName) {
    static QString dirMime = QStringLiteral("vnd.android.document/directory");
    return createFile(parentDirUri, subDirName, dirMime);
}

QString AndroidStorage::createFile(const QString &parentDirUri, const QString &fileName, const QString &mimeType) {
    QJniObject parentUriObj = getUri(parentDirUri);
    if (isDir(parentUriObj) == false) {
        return QString();
    }
    QJniObject parentDocUri = getDocumentUri(parentUriObj);
    if (parentDocUri.isValid() == false) {
        return QString();
    }
    QJniObject subDirUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "createDocument", "(Landroid/content/ContentResolver;Landroid/net/Uri;Ljava/lang/String;Ljava/lang/String;)Landroid/net/Uri;", getContentResolver().object(), parentDocUri.object(), QJniObject::fromString(mimeType).object<jstring>(), QJniObject::fromString(fileName).object<jstring>());
    clearExceptions();
    return subDirUri.toString();
}

bool AndroidStorage::removeFile(const QString &uri) {
    QJniObject uriObj = getUri(uri);
    if (uriObj.isValid() == false) {
        return false;
    }
    QJniObject docUri = getDocumentUri(uriObj);
    if (docUri.isValid() == false) {
        return false;
    }
    return QJniObject::callStaticMethod<jboolean>("android/provider/DocumentsContract", "deleteDocument", "(Landroid/content/ContentResolver;Landroid/net/Uri;)Z", getContentResolver().object(), docUri.object());
}

#endif
