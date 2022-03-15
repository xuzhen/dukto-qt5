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
    clip: true
    interactive: (lastItem.y + lastItem.height + 20) > height
    flickableDirection: Flickable.VerticalFlick
    contentHeight: lastItem.y + lastItem.height + 20
    boundsBehavior: Flickable.StopAtBounds

    Item {
        id: container
        anchors.left: parent.left
        anchors.right: parent.right

        Rectangle {
            x: 27
            y: 15
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
            y: 80
            x: 22
            font.pixelSize: 100
            text: "Dukto R6"
            color: "#555555"
        }
        SmoothText {
            x: 25
            y: 70 + 75
            font.pixelSize: 38
            text: "Created by Emanuele Colombo"
            color: "#888888"
        }
        SmoothText {
            x: 25
            y: 100 + 75
            font.pixelSize: 32
            text: "Website: <a href=\"https://www.msec.it/blog/dukto/\">https://www.msec.it/blog/dukto/</a>"
            color: "#888888"
            Connections {
                function onLinkActivated(link) {
                    console.log(root.height, container.height, lastItem.y)
                    Qt.openUrlExternally(link)
                }
            }
        }

        SText {
            id: lastItem
            anchors.right: parent.right
            anchors.rightMargin: 40
            anchors.left: parent.left
            anchors.leftMargin: 25
            y: 240
            font.pixelSize: 12
            color: "#888888"
            wrapMode: Text.WordWrap
            text: "This application and it's source code are released freely as open source project.<br>If you like this application please consider <a href=\"https://www.msec.it/\">making a donation</a>."
            Connections {
                function onLinkActivated(link) {
                    Qt.openUrlExternally(link)
                }
            }
        }
    }
}
