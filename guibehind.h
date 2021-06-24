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
class Settings;
class DuktoWindow;
class SystemTray;

class GuiBehind : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTransferBuddy READ currentTransferBuddy NOTIFY currentTransferBuddyChanged)
    Q_PROPERTY(int currentTransferProgress READ currentTransferProgress NOTIFY currentTransferProgressChanged)
    Q_PROPERTY(QString currentTransferStats READ currentTransferStats NOTIFY currentTransferStatsChanged)
    Q_PROPERTY(bool currentTransferSending READ currentTransferSending NOTIFY currentTransferSendingChanged)
    Q_PROPERTY(QString textSnippetBuddy READ textSnippetBuddy NOTIFY textSnippetBuddyChanged)
    Q_PROPERTY(QString textSnippet READ textSnippet WRITE setTextSnippet NOTIFY textSnippetChanged)
    Q_PROPERTY(bool textSnippetSending READ textSnippetSending NOTIFY textSnippetSendingChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(bool clipboardTextAvailable READ clipboardTextAvailable NOTIFY clipboardTextAvailableChanged)
    Q_PROPERTY(QString remoteDestinationAddress READ remoteDestinationAddress WRITE setRemoteDestinationAddress NOTIFY remoteDestinationAddressChanged)
    Q_PROPERTY(QString overlayState READ overlayState WRITE setOverlayState NOTIFY overlayStateChanged)
    Q_PROPERTY(QString messagePageText READ messagePageText WRITE setMessagePageText NOTIFY messagePageTextChanged)
    Q_PROPERTY(QString messagePageTitle READ messagePageTitle WRITE setMessagePageTitle NOTIFY messagePageTitleChanged)
    Q_PROPERTY(QString messagePageBackState READ messagePageBackState WRITE setMessagePageBackState NOTIFY messagePageBackStateChanged)
    Q_PROPERTY(bool showTermsOnStart READ showTermsOnStart WRITE setShowTermsOnStart NOTIFY showTermsOnStartChanged)
    Q_PROPERTY(bool showUpdateBanner READ showUpdateBanner WRITE setShowUpdateBanner NOTIFY showUpdateBannerChanged)
    Q_PROPERTY(QString buddyName READ buddyName WRITE setBuddyName NOTIFY buddyNameChanged)

public:
    explicit GuiBehind(Settings* settings);
    virtual ~GuiBehind();

    void setViewer(DuktoWindow *view, SystemTray *tray);
    bool canAcceptDrop();
    void sendDroppedFiles(QStringList *files);
    void close();

    QString currentTransferBuddy();
    void setCurrentTransferBuddy(QString buddy);
    int currentTransferProgress();
    void setCurrentTransferProgress(int value);
    QString currentTransferStats();
    void setCurrentTransferStats(QString stats);
    QString textSnippetBuddy();
    void setTextSnippetBuddy(QString buddy);
    QString textSnippet();
    void setTextSnippet(QString txt);
    bool textSnippetSending();
    void setTextSnippetSending(bool sending);
    QString currentPath();
    void setCurrentPath(QString path);
    bool currentTransferSending();
    void setCurrentTransferSending(bool sending);
    bool clipboardTextAvailable();
    QString remoteDestinationAddress();
    void setRemoteDestinationAddress(QString address);
    QString overlayState();
    void setOverlayState(QString state);
    QString messagePageText();
    void setMessagePageText(QString message);
    QString messagePageTitle();
    void setMessagePageTitle(QString title);
    QString messagePageBackState();
    void setMessagePageBackState(QString state);
    bool showTermsOnStart();
    void setShowTermsOnStart(bool show);
    bool showUpdateBanner();
    void setShowUpdateBanner(bool show);
    void setBuddyName(QString name);
    QString buddyName();

protected:
    bool eventFilter(QObject *, QEvent *event);

signals:
    void currentTransferBuddyChanged();
    void currentTransferProgressChanged();
    void currentTransferStatsChanged();
    void currentTransferSendingChanged();
    void textSnippetBuddyChanged();
    void textSnippetChanged();
    void textSnippetSendingChanged();
    void currentPathChanged();
    void clipboardTextAvailableChanged();
    void remoteDestinationAddressChanged();
    void overlayStateChanged();
    void messagePageTextChanged();
    void messagePageTitleChanged();
    void messagePageBackStateChanged();
    void showTermsOnStartChanged();
    void showUpdateBannerChanged();
    void buddyNameChanged();

    // Received by QML
    void transferStart();
    void receiveCompleted();
    void gotoTextSnippet();
    void gotoSendPage();
    void gotoMessagePage();
    void hideAllOverlays();

public slots:
    void showRandomBack();
    void clipboardChanged();
    void remoteDestinationAddressHandler();
    void periodicHello();
    void showUpdatesMessage();
    void sendScreenStage2();

    // Called by Dukto protocol
    void peerListAdded(Peer peer);
    void peerListRemoved(Peer peer);
    void receiveFileStart(QString senderIp);
    void transferStatusUpdate(qint64 total, qint64 partial);
    void receiveFileComplete(QStringList *files, qint64 totalSize);
    void receiveTextComplete(QString *text, qint64 totalSize);
    void sendFileComplete();
    void sendFileError(int code);
    void receiveFileCancelled();
    void sendFileAborted();

    // Called by QML
    void openDestinationFolder();
    void refreshIpList();
    void showTextSnippet(QString text, QString sender);
    void openFile(QString path);
    void changeDestinationFolder();
    void showSendPage(QString ip);
    void sendSomeFiles();
    void sendFolder();
    void sendClipboardText();
    void sendText();
    void sendScreen();
    void changeThemeColor(QString color);
    void resetProgressStatus();
    void abortTransfer();

private:
    DuktoWindow *mView;
    QTimer *mShowBackTimer;
    QTimer *mPeriodicHelloTimer;
    Settings *mSettings;
    DestinationBuddy *mDestBuddy;
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

    bool prepareStartTransfer(QString *ip, qint16 *port);
    void startTransfer(QStringList files);
    void startTransfer(QString text);
};

#endif // GUIBEHIND_H
