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

import QtQuick 2.3

Flickable {
    property var lastItem: authors
    clip: true
    interactive: (lastItem.y + lastItem.height + 20) > height
    flickableDirection: Flickable.VerticalFlick
    contentHeight: lastItem.y + lastItem.height + 20
    boundsBehavior: Flickable.StopAtBounds

    Item {
        id: container
        anchors.fill: parent
        anchors.leftMargin: 35
        anchors.rightMargin: 35

        Rectangle {
            id: logo
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 3
            width: 64
            height: 64
            color: theme.mainColor
            Image {
                source: "TileGradient.png"
                anchors.fill: parent
            }
            Image {
                source: "DuktoMetroIcon.png"
                anchors.fill: parent
            }
        }

        SmoothText {
            id: name
            anchors.left: parent.left
            anchors.top: logo.bottom
            anchors.topMargin: 10
            font.pixelSize: 36
            scale: 1
            text: "Dukto"
            color: "#555555"
        }
        SText {
            anchors.left: name.right
            anchors.leftMargin: 10
            anchors.bottom: name.bottom
            font.pixelSize: 32
            text: guiBehind.version()
            color: "#555555"
        }

        SmoothText {
            id: website
            anchors.left: parent.left
            anchors.top: name.bottom
            anchors.topMargin: 10
            font.pixelSize: 18
            scale: 1
            text: "Website: <a href=\"https://github.com/xuzhen/dukto-qt5/\">Source Code Repository</a>"
            color: "#555555"
            linkColor: theme.mainColor
            onLinkActivated: Qt.openUrlExternally(link)
        }

        SmoothText {
            id: copyrights
            anchors.left: parent.left
            anchors.top: website.bottom
            anchors.topMargin: 10
            font.pixelSize: 18
            scale: 1
            text: "Created by:"
            color: "#555555"
        }
        SmoothText {
            id: authors
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: copyrights.bottom
            anchors.topMargin: 6
            font.pixelSize: 16
            lineHeight: 1.2
            scale: 1
            text: "2009-2013 © <a href=\"https://www.msec.it/blog/dukto/\">Emanuele Colombo</a><br>2015-2015 © <a href=\"https://github.com/arthurzam/\">Arthur Zamarin</a><br>2015-2021 © <a href=\"https://github.com/maz-1/\">maz-1</a><br>2021-2022 © <a href=\"https://github.com/xuzhen/\">Xu Zhen</a>"
            color: "#555555"
            linkColor: theme.mainColor
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}
