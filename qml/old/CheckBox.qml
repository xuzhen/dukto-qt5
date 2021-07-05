/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2021 Xu Zhen
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

Rectangle {
    id: checkbox
    property bool checked: false
    property alias text: label.text

    signal clicked(bool checked)

    Rectangle {
        id: indicator
        implicitWidth: 16
        implicitHeight: 16
        border.color: "#888888"
        border.width: 1

        Rectangle {
            visible: checkbox.checked
            color: theme.color2
            anchors.margins: 3
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: {
                checkbox.checked = !checkbox.checked;
                checkbox.clicked(checkbox.checked)
            }
        }
    }

    SText {
        id: label
        font.pixelSize: 16
        color: "#888888"
        anchors.top: checkbox.top
        anchors.left: indicator.right
        anchors.leftMargin: 4
        anchors.topMargin: -1

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: {
                checkbox.checked = !checkbox.checked;
                checkbox.clicked(checkbox.checked)
            }
        }
    }
}
