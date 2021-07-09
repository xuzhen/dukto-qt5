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

#include "duktowindow.h"
#include "guibehind.h"
#include "platform.h"
#include "settings.h"

#ifdef Q_OS_WIN
#include "ecwin7.h"
#endif

#include <QQmlEngine>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>

#ifdef Q_OS_MAC
#include <objc/objc.h>
#include <objc/message.h>
static DuktoWindow *instance = nullptr;
#endif

DuktoWindow::DuktoWindow(GuiBehind *gb, Settings *settings, QWidget *parent) :
    QQuickWidget(parent), mGuiBehind(gb), mSettings(settings)
{
    // Configure window
    setAcceptDrops(true);
    setWindowTitle("Dukto");
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setMaximumSize(350, 5000);
    setMinimumSize(350, 500);
    setWindowIcon(QIcon(":/dukto.png"));
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    connect(engine(), &QQmlEngine::quit, this, &DuktoWindow::close);
#ifdef Q_OS_MAC
    instance = this;
    setupDockHandler();
#endif
}

DuktoWindow::~DuktoWindow() {
#ifdef Q_OS_WIN
    delete mWin7;
#endif
}

void DuktoWindow::showTaskbarProgress(uint percent) {
#ifdef Q_OS_WIN
    if (mWin7 != nullptr) {
        mWin7->setProgressState(EcWin7::Normal);
        mWin7->setProgressValue(percent, 100);
    }
#else
    Q_UNUSED(percent)
#endif
}

void DuktoWindow::hideTaskbarProgress() {
#ifdef Q_OS_WIN
    if (mWin7 != nullptr) {
        mWin7->setProgressState(EcWin7::NoProgress);
    }
#endif
}

void DuktoWindow::stopTaskbarProgress() {
#ifdef Q_OS_WIN
    if (mWin7 != nullptr) {
        mWin7->setProgressState(EcWin7::Error);
    }
#endif
}

void DuktoWindow::activateWindow() {
    showNormal();
    raise();
    QQuickWidget::activateWindow();
}

#ifdef Q_OS_WIN
bool DuktoWindow::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    Q_UNUSED(eventType)
    if (mWin7 != nullptr) {
        return mWin7->winEvent(reinterpret_cast<MSG*>(message), result);
    } else {
        return false;
    }
}
#endif

void DuktoWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Back) {
        QString state = mGuiBehind->overlayState();
        if (state == QStringLiteral("progress")) {
            event->accept();
            return;
        } else if (state == QStringLiteral("message") && mGuiBehind->messagePageBackState() == QStringLiteral("send")) {
            emit mGuiBehind->gotoSendPage();
            event->accept();
            return;
        } else if (!state.isEmpty() && state != QStringLiteral("termspage")) {
            emit mGuiBehind->hideAllOverlays();
            event->accept();
            return;
        }
    }
    QQuickWidget::keyPressEvent(event);
}

void DuktoWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && mGuiBehind->canAcceptDrop())
        event->acceptProposedAction();
}

void DuktoWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void DuktoWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void DuktoWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if (!mimeData->hasUrls()) return;

    QStringList files;
    const QList<QUrl> urlList = mimeData->urls();
    for(QList<QUrl>::const_iterator url = urlList.constBegin(); url != urlList.constEnd(); ++url)
        files.append(url->toLocalFile());

    event->acceptProposedAction();
    mGuiBehind->sendDroppedFiles(&files);
}

void DuktoWindow::closeEvent(QCloseEvent *event)
{
    if (isVisible() && mSettings->closeToTrayEnabled()) {
        event->ignore();
        hide();
    } else {
        mSettings->saveWindowGeometry(saveGeometry());
        event->accept();
    }
}

void DuktoWindow::showEvent(QShowEvent *event) {
    QQuickWidget::showEvent(event);
#ifdef Q_OS_WIN
    // Taskbar integration with Win7+
    if (mWin7 == nullptr) {
        mWin7 = new EcWin7(this->windowHandle());
    }
#endif
}

#ifdef Q_OS_MAC
bool dockHasVisibleWindows(id self, SEL _cmd, id sender, bool flag) {
    Q_UNUSED(self)
    Q_UNUSED(_cmd)
    Q_UNUSED(sender)
    if (!flag) {
        // window is minimized to tray (hidden)
        instance->activateWindow();
        return false;
    }
    return true;
}

void DuktoWindow::setupDockHandler() {
    Class appClass = objc_getClass("NSApplication");
    id appInst = reinterpret_cast<id(*)(Class, SEL)>(objc_msgSend)(appClass, sel_registerName("sharedApplication"));
    if (appInst != nullptr) {
        id delegate = reinterpret_cast<id(*)(id, SEL)>(objc_msgSend)(appInst, sel_registerName("delegate"));
        if (delegate != nullptr) {
            Class delClass = reinterpret_cast<Class(*)(id, SEL)>(objc_msgSend)(delegate, sel_registerName("class"));
            if (delClass != nullptr) {
                SEL methodSelector = sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:");
                class_replaceMethod(delClass, methodSelector, reinterpret_cast<IMP>(dockHasVisibleWindows), "B@:@B");
            }
        }
    }
}
#endif
