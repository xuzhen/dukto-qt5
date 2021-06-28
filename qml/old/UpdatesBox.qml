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

Item {
    id: box
    width: 190
    height: 55
    clip: true
    state: guiBehind.showUpdateBanner ? "showed" : "hidden"

    Rectangle {
        id: backRecangle
        color: theme.color2
        height: parent.height
        width: parent.width

        Image {
            anchors.fill: parent
            source: "TileGradient.png"
        }

        SText {
            id: labelText
            anchors.fill: parent
            anchors.margins: 10
            font.pixelSize: 14
            wrapMode: "WordWrap"
            text: "A new release is available, click here to download it!"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.openUrlExternally("http://www.msec.it/dukto/r5update.php")
        }

        Behavior on x { NumberAnimation { duration: 500; easing.type: "OutCubic" } }
    }
    states: [
        State {
            name: "hidden"

            PropertyChanges {
                target: backRecangle
                x: box.width
            }
        },
        State {
            name: "showed"

            PropertyChanges {
                target: backRecangle
                x: 0
            }
        }
    ]


}
