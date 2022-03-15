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
 
#ifndef DUKTOWINDOW_H
#define DUKTOWINDOW_H

#include <QQuickWidget>

#ifdef Q_OS_WIN
class EcWin7;
#endif
class GuiBehind;

class DuktoWindow : public QQuickWidget
{
    Q_OBJECT
public:
    explicit DuktoWindow(GuiBehind* gb, QWidget *parent = nullptr);
    virtual ~DuktoWindow();
    void showTaskbarProgress(uint percent);
    void hideTaskbarProgress();
    void stopTaskbarProgress();

public slots:
    void activateWindow();

signals:
#ifdef Q_OS_ANDROID
    void cursorPositionChanged();
#endif

protected:
#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif
#endif
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    GuiBehind *mGuiBehind;
#ifdef Q_OS_WIN
    EcWin7 *mWin7 = nullptr;
#endif
#ifdef Q_OS_MAC
    void setupDockHandler();
#endif
};

#endif // DUKTOWINDOW_H
