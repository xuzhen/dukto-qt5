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


QString AndroidUtilsBase::getPackageName() {
    static QString packageName;
    if (packageName.isEmpty()) {
        packageName = getContext().callObjectMethod("getPackageName", "()Ljava/lang/String;").toString();
    }
    return packageName;
}


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
        context = QNativeInterface::QAndroidApplication::context();
#endif
    }
    return context;
}

QJniObject AndroidUtilsBase::getActivity() {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return QtAndroid::androidActivity();
#else
    return QNativeInterface::QAndroidApplication::context();
#endif
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

AndroidScreenOn::AndroidScreenOn() : window(getActivity().callObjectMethod("getWindow", "()Landroid/view/Window;")) {
    auto code = [this]() {
        // 128 = WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
        window.callMethod<void>("addFlags", "(I)V", static_cast<jint>(128));
    };
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread(code).waitForFinished();
#else
    QtAndroid::runOnAndroidThreadSync(code);
#endif
}

AndroidScreenOn::~AndroidScreenOn() {
    auto code = [this]() {
        // 128 = WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
        window.callMethod<void>("clearFlags", "(I)V", static_cast<jint>(128));
    };
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread(code).waitForFinished();
#else
    QtAndroid::runOnAndroidThreadSync(code);
#endif
}

/*============================================================*/

AndroidContentReader::AndroidContentReader(const QString &uri) : uriString(uri), uriObject(AndroidStorage::parseUri(uri)) {
}

AndroidContentReader::AndroidContentReader(const QJniObject &uri) : uriString(uri.toString()), uriObject(uri) {
}

AndroidContentReader::~AndroidContentReader() {
    close();
}

QString AndroidContentReader::getFileName() {
    return AndroidStorage::getFileName(uriObject);
}

QString AndroidContentReader::getUri() {
    return uriString;
}

qint64 AndroidContentReader::getAvaiableBytes() {
    if (stream == nullptr) {
        return -1;
    }
    qint64 size = stream->callMethod<jint>("available", "()I");
    clearExceptions();
    return size;
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
    int r = read(size, buffer.data());
    if (r == -1) {
        return QByteArray();
    }
    buffer.resize(r);
    return buffer;
}

int AndroidContentReader::read(int size, char* buffer) {
    if (stream == nullptr) {
        return -1;
    }
    QJniEnvironment env;
    jbyteArray array = env->NewByteArray(size);
    if (array == nullptr) {
        clearExceptions();
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

AndroidContentWriter::AndroidContentWriter(const QString &uri) : uriString(uri), uriObject(AndroidStorage::parseUri(uri)) {
}

AndroidContentWriter::AndroidContentWriter(const QJniObject &uri) : uriString(uri.toString()), uriObject(uri) {
}

AndroidContentWriter::~AndroidContentWriter() {
    close();
}

QString AndroidContentWriter::getFileName() {
    return AndroidStorage::getFileName(uriObject);
}

QString AndroidContentWriter::getUri() {
    return uriString;
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
    return !r;
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

QString AndroidStorage::dirMimeType = QStringLiteral("vnd.android.document/directory");

QJniObject AndroidStorage::parseUri(const QString &uriString) {
    QJniObject uri = QJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", QJniObject::fromString(uriString).object<jstring>());
    if (uri.isValid() == false) {
        clearExceptions();
    }
    return uri;
}

bool AndroidStorage::hasUriPermission(const QString &uri, bool writable) {
    QJniObject objectList = getContentResolver().callObjectMethod("getPersistedUriPermissions", "()Ljava/util/List;");
    if (objectList.isValid()) {
        jint size = objectList.callMethod<jint>("size");
        for (jint i = 0; i < size; i++) {
            QJniObject permission = objectList.callObjectMethod("get", "(I)Ljava/lang/Object;", i);
            if (permission.isValid() == false) {
                clearExceptions();
                continue;
            }
            QJniObject permissionUri = permission.callObjectMethod("getUri", "()Landroid/net/Uri;");
            if (uri != permissionUri.toString()) {
                continue;
            }
            if (writable) {
                if (permission.callMethod<jboolean>("isWritePermission")) {
                    return true;
                }
            } else {
                if (permission.callMethod<jboolean>("isReadPermission")) {
                    return true;
                }
            }
        }
    }
    clearExceptions();
    return false;
}

bool AndroidStorage::hasUriPermission(const QJniObject &uri, bool writable) {
    return hasUriPermission(uri.toString(), writable);
}

void AndroidStorage::grantUriPermission(const QJniObject &uri, bool writable) {
    // 1  = Intent.FLAG_GRANT_READ_URI_PERMISSION
    // 2  = Intent.FLAG_GRANT_WRITE_URI_PERMISSION
    // 64 = Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION
    getContext().callMethod<void>("grantUriPermission", "(Ljava/lang/String;Landroid/net/Uri;I)V", QJniObject::fromString(getPackageName()).object<jstring>(), uri.object(), (writable ? (1 | 2 | 64) : (1 | 64)));
    getContentResolver().callMethod<void>("takePersistableUriPermission", "(Landroid/net/Uri;I)V", uri.object(), (writable ? (1 | 2) : 1));
    clearExceptions();
}

void AndroidStorage::revokeUriPermission(const QJniObject &uri) {
    // 1  = Intent.FLAG_GRANT_READ_URI_PERMISSION
    // 2  = Intent.FLAG_GRANT_WRITE_URI_PERMISSION
    getContentResolver().callMethod<void>("releasePersistableUriPermission", "(Landroid/net/Uri;I)V", uri.object(), 1 | 2);
    clearExceptions();
}

bool AndroidStorage::isDir(const QJniObject &uri) {
    if (uri.isValid() == false) {
        return false;
    }
    return (getMimeType(getDocumentUri(uri)) == dirMimeType);
}

bool AndroidStorage::exists(const QJniObject &parentDirUri, const QString &fileName, Qt::CaseSensitivity cs) {
    QJniObject childrenUri = getChildDocumentsUri(parentDirUri);
    if (childrenUri.isValid() == false) {
        return false;
    }
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", childrenUri.object(), nullptr, nullptr, nullptr, nullptr);
    if (cursor.isValid()) {
        jint nameIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", QJniObject::fromString("_display_name").object<jstring>());
        if (nameIndex != -1) {
            while (cursor.callMethod<jboolean>("moveToNext")) {
                QString docName = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", nameIndex).toString();
                if (fileName.compare(docName, cs) == 0) {
                    cursor.callMethod<void>("close");
                    clearExceptions();
                    return true;
                }
            }
        }
        cursor.callMethod<void>("close");
    }
    clearExceptions();
    return false;
}


QJniObject AndroidStorage::getEntry(const QJniObject &parentDirUri, const QString &childDirName, Qt::CaseSensitivity cs) {
    QJniObject childrenUri = getChildDocumentsUri(parentDirUri);
    if (childrenUri.isValid() == false) {
        return QJniObject();
    }
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", childrenUri.object(), nullptr, nullptr, nullptr, nullptr);
    if (cursor.isValid()) {
        jint idIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", QJniObject::fromString("document_id").object<jstring>());
        jint nameIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", QJniObject::fromString("_display_name").object<jstring>());
        if (idIndex != -1 && nameIndex != -1) {
            while (cursor.callMethod<jboolean>("moveToNext")) {
                QString docName = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", nameIndex).toString();
                if (childDirName.compare(docName, cs) == 0) {
                    QJniObject childDocId = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", idIndex);
                    QJniObject childDocUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "buildDocumentUriUsingTree", "(Landroid/net/Uri;Ljava/lang/String;)Landroid/net/Uri;", childrenUri.object(), childDocId.object<jstring>());
                    cursor.callMethod<void>("close");
                    clearExceptions();
                    return childDocUri;
                }
            }
        }
        cursor.callMethod<void>("close");
    }
    clearExceptions();
    return QJniObject();
}


QList<QJniObject> AndroidStorage::getEntryList(const QJniObject &dirUri) {
    QJniObject childrenUri = getChildDocumentsUri(dirUri);
    if (childrenUri.isValid() == false) {
        return QList<QJniObject>();
    }
    QList<QJniObject> entries;
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", childrenUri.object(), nullptr, nullptr, nullptr, nullptr);
    if (cursor.isValid()) {
        jint idIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", QJniObject::fromString("document_id").object<jstring>());
        if (idIndex != -1) {
            while (cursor.callMethod<jboolean>("moveToNext")) {
                QJniObject childDocId = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", idIndex);
                QJniObject childDocUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "buildDocumentUriUsingTree", "(Landroid/net/Uri;Ljava/lang/String;)Landroid/net/Uri;", childrenUri.object(), childDocId.object<jstring>());
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

QString AndroidStorage::getFileName(const QJniObject &uri) {
    QJniObject docUri = getDocumentUri(uri);
    if (docUri.isValid() == false) {
        return QString();
    }
    QString filename;
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", docUri.object(), nullptr, nullptr, nullptr, nullptr);
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
        filename = docUri.toString().section(QChar('/'), -1);
    } else if (filename.contains(QChar(':'))) {

    }
    return filename;
}

qint64 AndroidStorage::getSize(const QJniObject &uri) {
    QJniObject docUri = getDocumentUri(uri);
    if (docUri.isValid() == false) {
        return -1;
    }
    qint64 size = -1;
    QJniObject cursor = getContentResolver().callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", docUri.object(), nullptr, nullptr, nullptr, nullptr);
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
    return size;
}

QString AndroidStorage::getMimeType(const QJniObject &uri) {
    if (uri.isValid() == false) {
        return QString();
    }
    QString mimeType = getContentResolver().callObjectMethod("getType", "(Landroid/net/Uri;)Ljava/lang/String;", uri.object()).toString();
    clearExceptions();
    return mimeType;
}


QJniObject AndroidStorage::getDocumentUri(const QJniObject &uri) {
    if (uri.isValid() == false) {
        return QJniObject();
    }
    if (QJniObject::callStaticMethod<jboolean>("android/provider/DocumentsContract", "isDocumentUri", "(Landroid/content/Context;Landroid/net/Uri;)Z", getContext().object(), uri.object())) {
        clearExceptions();
        return uri;
    }
    QJniObject docId = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "getTreeDocumentId", "(Landroid/net/Uri;)Ljava/lang/String;", uri.object());
    if (!docId.isValid()) {
        clearExceptions();
        return QJniObject();
    }
    QJniObject docUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "buildDocumentUriUsingTree", "(Landroid/net/Uri;Ljava/lang/String;)Landroid/net/Uri;", uri.object(), docId.object<jstring>());
    if (!docUri.isValid()) {
        clearExceptions();
        return QJniObject();
    }
    return docUri;
}

QJniObject AndroidStorage::getChildDocumentsUri(const QJniObject &uri) {
    QJniObject docUri = getDocumentUri(uri);
    if (docUri.isValid() == false) {
        return QJniObject();
    }
    if (isDir(docUri) == false) {
        return QJniObject();
    }
    QJniObject docId = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "getDocumentId", "(Landroid/net/Uri;)Ljava/lang/String;", docUri.object());
    if (docId.isValid() == false) {
        clearExceptions();
        return QJniObject();
    }
    QJniObject childrenUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "buildChildDocumentsUriUsingTree", "(Landroid/net/Uri;Ljava/lang/String;)Landroid/net/Uri;", docUri.object(), docId.object<jstring>());
    if (childrenUri.isValid() == false) {
        clearExceptions();
        return QJniObject();
    }
    return childrenUri;
}

QJniObject AndroidStorage::createDir(const QJniObject &parentDirUri, const QString &childDirName) {
    QJniObject uri = getEntry(parentDirUri, childDirName);
    if (uri.isValid()) {
        if (isDir(uri)) {
            return uri;
        } else {
            return QJniObject();
        }
    }
    return createFile(parentDirUri, childDirName, dirMimeType);
}

QJniObject AndroidStorage::createPath(const QJniObject &parentDirUri, const QStringList &path) {
    QJniObject uri = getDocumentUri(parentDirUri);
    if (uri.isValid() == false) {
        return QJniObject();
    }
    for (const QString &dir: path) {
        if (dir.isEmpty()) {
            continue;
        }
        uri = AndroidStorage::createDir(uri, dir);
        if (uri.isValid() == false) {
            return QJniObject();
        }
    }
    return uri;
}

QJniObject AndroidStorage::createFile(const QJniObject &parentDirUri, const QString &fileName, const QString &mimeType) {
    QJniObject parentDocUri = getDocumentUri(parentDirUri);
    if (parentDocUri.isValid() == false) {
        return QJniObject();
    }
    if (isDir(parentDocUri) == false) {
        return QJniObject();
    }
    QJniObject subDirUri = QJniObject::callStaticObjectMethod("android/provider/DocumentsContract", "createDocument", "(Landroid/content/ContentResolver;Landroid/net/Uri;Ljava/lang/String;Ljava/lang/String;)Landroid/net/Uri;", getContentResolver().object(), parentDocUri.object(), QJniObject::fromString(mimeType).object<jstring>(), QJniObject::fromString(fileName).object<jstring>());
    clearExceptions();
    return subDirUri;
}

bool AndroidStorage::removeFile(const QJniObject &uri) {
    if (uri.isValid() == false) {
        return false;
    }
    QJniObject docUri = getDocumentUri(uri);
    if (docUri.isValid() == false) {
        return false;
    }
    return QJniObject::callStaticMethod<jboolean>("android/provider/DocumentsContract", "deleteDocument", "(Landroid/content/ContentResolver;Landroid/net/Uri;)Z", getContentResolver().object(), docUri.object());
}

#endif
