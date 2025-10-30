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
    id: toolBar
    height: 53
    state: "WithoutLabels"

    signal clicked(string command)

    Behavior on height { NumberAnimation { duration: 200; easing.type: "OutCubic" } }

    Image {
        source: "BottomShadow.png"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        fillMode: Image.TileHorizontally
    }

    Rectangle {
        id: bar
        y: 3
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: theme.mainColor

        Behavior on height { NumberAnimation { duration: 200; easing.type: "OutCubic" } }

        Image {
            anchors.top: parent.top
            anchors.left: parent.left
            source: "PanelGradient.png"
        }

        Image {
            id: openFolderIcon
            visible: guiBehind.isDesktopApp()
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.right: showIpIcon.left
            anchors.rightMargin: 40
            source: "OpenFolderIcon.png"

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                onClicked: guiBehind.openDestinationFolder();
            }
        }
        SText {
            visible: guiBehind.isDesktopApp()
            anchors.top: openFolderIcon.bottom
            anchors.topMargin: 3
            anchors.horizontalCenter: openFolderIcon.horizontalCenter
            text: "Received"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            width: 1
        }

        Image {
            id: showIpIcon
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: guiBehind.isDesktopApp() ? 0 : -40
            source: "ShowIpIcon.png"

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                onClicked: {
                    guiBehind.refreshIpList();
                    toolBar.clicked("ip");
                }
            }
        }
        SText {
            anchors.top: showIpIcon.bottom
            anchors.topMargin: 3
            anchors.horizontalCenter: showIpIcon.horizontalCenter
            text: "Addresses"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            width: 1
        }

        Image {
            id: configIcon
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: showIpIcon.right
            anchors.leftMargin: 40
            source: "ConfigIcon.png"

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                onClicked: toolBar.clicked("settings")
            }
        }
        SText {
            anchors.top: configIcon.bottom
            anchors.topMargin: 3
            anchors.horizontalCenter: configIcon.horizontalCenter
            text: "Settings"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            width: 1
        }

        Image {
            id: moreIcon
            anchors.top: parent.top
            anchors.topMargin: 4
            anchors.right: parent.right
            anchors.rightMargin: 12
            source: "MoreIcon.png"
        }
        MouseArea {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: moreIcon.left
            anchors.right: parent.right
            hoverEnabled: true
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor

            onClicked: {
               if (toolBar.state == "WithoutLabels")
                   toolBar.state = "WithLabels";
               else
                   toolBar.state = "WithoutLabels";
           }
        }
    }

    states: [
        State {
            name: "WithoutLabels"

            PropertyChanges {
                target: toolBar
                height: 53
            }

            PropertyChanges {
                target: bar
                height: 50
            }
        },
        State {
            name: "WithLabels"

            PropertyChanges {
                target: toolBar
                height: 71
            }

            PropertyChanges {
                target: bar
                height: 68
            }
        }
    ]
}
