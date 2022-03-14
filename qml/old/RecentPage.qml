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
    id: recentPage
    clip: true

    signal showText();

    SmoothText {
        id: badSmile
        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.top: parent.top
        font.pixelSize: 160
        text: ":-("
        color: "#555555"
        visible: (recentList.count == 0)
    }

    SText  {
        y: badSmile.y + badSmile.height / 2 + 20
        // anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 40
        font.pixelSize: 17
        color: "#555555"
        text: "Sorry, no data has been\nreceived yet..."
        visible: (recentList.count == 0)
    }

    ListView {
        id: recentList
        anchors.fill: parent
        spacing: 10
        anchors.leftMargin: 25
        model: recentListData // EsempioRecent {}

        Component {
             id: recentDelegate
             Item {
                 anchors.left: parent.left
                 anchors.right: parent.right
                 width: parent.width
                 height: 36

                 MouseArea {
                     id: recentMouseArea
                     anchors.fill: parent
                     hoverEnabled: true
                     onClicked: {
                         if (type === "text")
                            guiBehind.showTextSnippet(value, sender);
                         else if (type === "file" || type === "dir")
                            guiBehind.openFile(value);
                     }
                 }

                 Rectangle {
                     anchors.fill: parent
                     color: "#00000000"
                     visible: recentMouseArea.containsMouse;
                     Rectangle {
                         anchors.right: parent.right
                         anchors.top: parent.top
                         anchors.bottom: parent.bottom
                         width: 5
                         color: theme.lighterColor
                     }
                 }

                 Rectangle {
                     id: fileImage
                     anchors.verticalCenter: parent.verticalCenter
                     width: 32
                     height: 32
                     color: theme.mainColor
                     Image {
                         source: typeIcon
                         width: 32
                         height: 32
                     }
                 }
                 SText {
                     id: recentNameText
                     anchors.top: fileImage.top
                     anchors.topMargin: -2
                     anchors.left: fileImage.right
                     anchors.leftMargin: 10
                     anchors.right: parent.right
                     anchors.rightMargin: 20
                     font.pixelSize: 14
                     color: "#555555"
                     elide: Text.ElideRight
                     text: name
                 }
                 SText {
                     anchors.left: recentNameText.left
                     anchors.leftMargin: 0
                     anchors.right: parent.right
                     anchors.rightMargin: 20
                     anchors.bottom: fileImage.bottom
                     anchors.bottomMargin: -1
                     font.pixelSize: 12
                     elide: Text.ElideRight
                     color: "#888888"
                     text: "from " + sender + ", " + dateTime
                 }
             }
         }

        delegate: recentDelegate
    }
}
