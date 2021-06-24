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

#include "ipaddressitemmodel.h"

#include <QHostAddress>
#include <QNetworkInterface>

IpAddressItemModel::IpAddressItemModel() :
    QStandardItemModel(NULL)
{
    QHash<int, QByteArray> roleNames;
    roleNames[Ip] = "ip";
    setItemRoleNames(roleNames);

    refreshIpList();
}

void IpAddressItemModel::addIp(QString ip)
{
    QStandardItem* it = new QStandardItem();
    it->setData(ip, IpAddressItemModel::Ip);
    appendRow(it);
}

void IpAddressItemModel::refreshIpList()
{
    // Clear current IP list
    clear();

    // Load IP list
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    for(QList<QHostAddress>::iterator iter = addrs.begin(); iter != addrs.end(); ++iter)
    {
        if(iter->protocol() == QAbstractSocket::IPv4Protocol)
        {
            QString addr = iter->toString();
            if(addr != "127.0.0.1")
                addIp(addr);
        }
    }
}
