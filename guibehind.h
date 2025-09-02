/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
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

#ifndef GUIBEHIND_H
#define GUIBEHIND_H

#include <QObject>

#include "buddylistitemmodel.h"
#include "recentlistitemmodel.h"
#include "ipaddressitemmodel.h"
#include "destinationbuddy.h"
#include "duktoprotocol.h"
#include "theme.h"

#ifdef UPDATER
class UpdatesChecker;
#endif
class MiniWebServer;
class DuktoWindow;
class SystemTray;

class GuiBehind : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTransferBuddy READ currentTransferBuddy NOTIFY currentTransferBuddyChanged)
    Q_PROPERTY(int currentTransferProgress READ currentTransferProgress NOTIFY currentTransferProgressChanged)
    Q_PROPERTY(QString currentTransferStats READ currentTransferStats NOTIFY currentTransferStatsChanged)
    Q_PROPERTY(QString currentTransferItem READ currentTransferItem NOTIFY currentTransferItemChanged)
    Q_PROPERTY(bool currentTransferSending READ currentTransferSending NOTIFY currentTransferSendingChanged)
    Q_PROPERTY(QString textSnippetBuddy READ textSnippetBuddy NOTIFY textSnippetBuddyChanged)
    Q_PROPERTY(QString textSnippet READ textSnippet WRITE setTextSnippet NOTIFY textSnippetChanged)
    Q_PROPERTY(bool textSnippetSending READ textSnippetSending NOTIFY textSnippetSendingChanged)
    Q_PROPERTY(QString destPath READ destPath NOTIFY destPathChanged)
    Q_PROPERTY(bool clipboardTextAvailable READ clipboardTextAvailable NOTIFY clipboardTextAvailableChanged)
    Q_PROPERTY(QString remoteDestinationAddress READ remoteDestinationAddress WRITE setRemoteDestinationAddress NOTIFY remoteDestinationAddressChanged)
    Q_PROPERTY(QString overlayState READ overlayState WRITE setOverlayState NOTIFY overlayStateChanged)
    Q_PROPERTY(QString messagePageText READ messagePageText WRITE setMessagePageText NOTIFY messagePageTextChanged)
    Q_PROPERTY(QString messagePageTitle READ messagePageTitle WRITE setMessagePageTitle NOTIFY messagePageTitleChanged)
    Q_PROPERTY(QString messagePageBackState READ messagePageBackState WRITE setMessagePageBackState NOTIFY messagePageBackStateChanged)
    Q_PROPERTY(bool showTermsOnStart READ showTermsOnStart WRITE setShowTermsOnStart NOTIFY showTermsOnStartChanged)
    Q_PROPERTY(bool showUpdateBanner READ showUpdateBanner WRITE setShowUpdateBanner NOTIFY showUpdateBannerChanged)
    Q_PROPERTY(QString buddyName READ buddyName WRITE setBuddyName NOTIFY buddyNameChanged)
    Q_PROPERTY(QString buddyAvatar READ buddyAvatar NOTIFY buddyAvatarChanged)
    Q_PROPERTY(bool showNotification READ showNotification WRITE setShowNotification NOTIFY showNotificationChanged)
    Q_PROPERTY(bool closeToTray READ closeToTray WRITE setCloseToTray NOTIFY closeToTrayChanged)
    Q_PROPERTY(QString initError READ initError NOTIFY initErrorChanged)
    Q_PROPERTY(QString initErrorAction READ initErrorAction NOTIFY initErrorActionChanged)

public:
    explicit GuiBehind();
    virtual ~GuiBehind();

    void setViewer(DuktoWindow *view, SystemTray *tray);
    bool canAcceptDrop();
    void sendDroppedFiles(QStringList *files);
    void close();

    QString currentTransferBuddy();
    void setCurrentTransferBuddy(const QString &buddy);
    int currentTransferProgress();
    void setCurrentTransferProgress(int value);
    QString currentTransferStats();
    void setCurrentTransferStats(const QString &stats);
    QString currentTransferItem();
    void setCurrentTransferItem(const QString &item);
    QString textSnippetBuddy();
    void setTextSnippetBuddy(const QString &buddy);
    QString textSnippet();
    void setTextSnippet(const QString &txt);
    bool textSnippetSending();
    void setTextSnippetSending(bool sending);
    QString destPath();
    void setDestPath(const QString &path);
    bool currentTransferSending();
    void setCurrentTransferSending(bool sending);
    bool clipboardTextAvailable();
    QString remoteDestinationAddress();
    void setRemoteDestinationAddress(const QString &address);
    QString overlayState();
    void setOverlayState(const QString &state);
    QString messagePageText();
    void setMessagePageText(const QString &message);
    QString messagePageTitle();
    void setMessagePageTitle(const QString &title);
    QString messagePageBackState();
    void setMessagePageBackState(const QString &state);
    bool showTermsOnStart();
    void setShowTermsOnStart(bool show);
    bool showUpdateBanner();
    void setShowUpdateBanner(bool show);
    void setBuddyName(const QString &name);
    QString buddyName();
    QString buddyAvatar();
    void setShowNotification(bool show);
    bool showNotification();
    void setCloseToTray(bool enabled);
    bool closeToTray();
    void setInitError(const QString &error, const QString &action = "Retry");
    QString initError();
    QString initErrorAction();

protected:
    bool eventFilter(QObject *, QEvent *event) override;

signals:
    void currentTransferBuddyChanged();
    void currentTransferProgressChanged();
    void currentTransferStatsChanged();
    void currentTransferItemChanged();
    void currentTransferSendingChanged();
    void textSnippetBuddyChanged();
    void textSnippetChanged();
    void textSnippetSendingChanged();
    void destPathChanged();
    void clipboardTextAvailableChanged();
    void remoteDestinationAddressChanged();
    void overlayStateChanged();
    void messagePageTextChanged();
    void messagePageTitleChanged();
    void messagePageBackStateChanged();
    void showTermsOnStartChanged();
    void showUpdateBannerChanged();
    void buddyNameChanged();
    void buddyAvatarChanged();
    void showNotificationChanged();
    void closeToTrayChanged();
    void initErrorChanged();
    void initErrorActionChanged();

    // Received by QML
    void transferStart();
    void receiveCompleted();
    void gotoTextSnippet();
    void gotoSendPage();
    void gotoMessagePage();
    void gotoProfilePage();
    void hideAllOverlays();

public slots:
    // Called by Dukto protocol
    void peerListAdded(const Peer &peer);
    void peerListRemoved(const Peer &peer);
    void receiveFileStart(const QString &senderIp);
    void transferStatusUpdate(qint64 total, qint64 partial);
    void transferItemUpdate(qint64 total, qint64 current, const QString &name);
    void receiveFileComplete(const QString &name, const QString &path, qint64 size);
    void receiveDirComplete(const QString &name, const QString &path);
    void receiveTextComplete(const QString &text);
    void receiveComplete();
    void sendFileComplete();
    void sendFileError(const QString &error);
    void receiveFileCancelled(const QString &error);
    void sendFileAborted();

    // Called by QML
    void openDestinationFolder();
    void refreshIpList();
    void showTextSnippet(const QString &text, const QString &sender);
    void openFile(const QString &path);
    void changeDestinationFolder();
    void showSendPage(const QString &ip);
    void sendSomeFiles();
    void sendFolder();
    void sendClipboardText();
    void sendText();
    void sendScreen();
    void changeThemeColor(const QString &color);
    void resetProgressStatus();
    void abortTransfer();
    bool isDesktopApp();
    void initialize();
    void reinitialize(const QString &action);
    void refreshNeighbors();
    void pasteDestinationIp();
    void showProfilePage();
    void selectAvatar();
    void resetBuddy();
    QString version();

private slots:
    void showRandomBack();
    void clipboardChanged();
    void remoteDestinationAddressHandler();
    void showUpdatesMessage();
    void sendScreenStage2();
    void discoveryNeighbors();

private:
    DuktoWindow *mView = nullptr;
    QTimer *mShowBackTimer = nullptr;
    QTimer *mPeriodicHelloTimer = nullptr;
    MiniWebServer *mMiniWebServer = nullptr;
    DestinationBuddy *mDestBuddy = nullptr;
    BuddyListItemModel mBuddiesList;
    RecentListItemModel mRecentList;
    IpAddressItemModel mIpAddresses;
    DuktoProtocol mDuktoProtocol;
    Theme mTheme;
#ifdef UPDATER
    UpdatesChecker *mUpdatesChecker;
#endif

    int mCurrentTransferProgress;
    QString mCurrentTransferBuddy;
    QString mCurrentTransferStats;
    QString mCurrentTransferItem;
    bool mCurrentTransferSending;
    QString mTextSnippetBuddy;
    QString mTextSnippet;
    bool mTextSnippetSending;
    bool mClipboardTextAvailable;
    QString mRemoteDestinationAddress;
    QString mOverlayState;
    QString mMessagePageText;
    QString mMessagePageTitle;
    QString mMessagePageBackState;
    bool mShowUpdateBanner;
    QString mScreenTempPath;
    QString mInitError;
    QString mInitErrorAction;

    bool testFolder(const QString &dir);
    bool prepareStartTransfer(QString *ip, qint16 *port);
    void startTransfer(const QStringList &files);
    void startTransfer(const QString &text);
};

#endif // GUIBEHIND_H
