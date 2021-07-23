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

AndroidUtilsBase::AndroidUtilsBase() {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QJniObject activity = QtAndroid::androidActivity();
    context = activity.callObjectMethod("getApplicationContext","()Landroid/content/Context;");
#else
    context = qApp->nativeInterface<QNativeInterface::QAndroidApplication>()->context();
#endif
}

/*
QString AndroidUtilsBase::getPackageName() {
    static QString packageName;
    if (packageName.isEmpty()) {
        packageName = context.callObjectMethod("getPackageName", "()Ljava/lang/String;").toString();
    }
    return packageName;
}
*/

QJniObject AndroidUtilsBase::getSystemService(const QString &name) {
    if (context.isValid()) {
        QJniObject serviceString = QJniObject::fromString(name);
        QJniObject service = context.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;", serviceString.object<jstring>());
        clearExceptions();
        return service;
    } else {
        return QJniObject();
    }
}

QJniObject AndroidUtilsBase::getContentResolver() {
    static QJniObject resolver;
    if (resolver.isValid() == false) {
        if (context.isValid()) {
            resolver = context.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
            clearExceptions();
        }
    }
    return resolver;
}

bool AndroidUtilsBase::clearExceptions() {
    QJniEnvironment env;
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
                                              getContentResolver().object<jobject>(),
                                              QJniObject::fromString(key).object<jstring>()).toString();
}

int32_t AndroidSettings::getIntValue(Scope scope, const QString &key) {
    return QJniObject::callStaticMethod<jint>(className(scope),
                                              "getInt",
                                              "(Landroid/content/ContentResolver;Ljava/lang/String;)I",
                                              getContentResolver().object<jobject>(),
                                              QJniObject::fromString(key).object<jstring>());
}

/*============================================================*/

AndroidMulticastLock::AndroidMulticastLock() {
    QJniObject wifiManager = getSystemService("wifi");
    if (wifiManager.isValid()) {
        lock = wifiManager.callObjectMethod("createMulticastLock", "(Ljava/lang/String;)Landroid/net/wifi/WifiManager$MulticastLock;", QJniObject::fromString("Dukto").object<jstring>());
    }
}

bool AndroidMulticastLock::acquire() {
    if (lock.isValid() == false) {
        return false;
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
    QtAndroid::PermissionResultMap result = QtAndroid::requestPermissionsSync(permissions);
    foreach (QtAndroid::PermissionResult r, result) {
        if (r != QtAndroid::PermissionResult::Granted) {
            return false;
        }
    }
    return true;
#else
    QFutureWatcher<QPermission::PermissionResult> watcher;
    foreach (QString p, permissions) {
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
    foreach (QString p, permissions) {
        if (QtAndroid::checkPermission(p) != QtAndroid::PermissionResult::Granted) {
            return false;
        }
    }
    return true;
#else
    QFutureWatcher<QPermission::PermissionResult> watcher;
    foreach (QString p, permissions) {
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
    foreach (QString u, contentUrls) {
        if (url.startsWith(u)) {
            return QDir(getExternalStorage()).filePath(QUrl::fromPercentEncoding(url.mid(u.length()).toUtf8()));
        }
    }
    return url;
}


#endif