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

#if defined(UPDATER) && !defined(UPDATESCHECKER_H)
#define UPDATESCHECKER_H

#include <QThread>

class QNetworkAccessManager;
class QNetworkReply;

class UpdatesChecker : public QThread
{
    Q_OBJECT
public:
    explicit UpdatesChecker(QObject *parent = nullptr);
    virtual ~UpdatesChecker();
    void run();

signals:
    void updatesAvailable();

private slots:
    void updatedDataReady(QNetworkReply *reply);

private:
    QNetworkAccessManager *mNetworkAccessManager = nullptr;
};

#endif
