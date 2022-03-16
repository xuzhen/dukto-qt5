/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2022 Xu Zhen
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
    id: profilePage
    color: "#00000000"

    signal back()

    MouseArea {
        anchors.fill: parent
    }

    Item {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        height: 270

        Rectangle {
            id: backRecangle
            color: theme.mainColor
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width * 2
        }

        Image {
            source: "BottomShadow.png"
            anchors.bottom: backRecangle.top
            anchors.left: backRecangle.left
            anchors.right: backRecangle.right
            fillMode: Image.TileHorizontally
        }

        Image {
            source: "TopShadow.png"
            anchors.top: backRecangle.bottom
            anchors.left: backRecangle.left
            anchors.right: backRecangle.right
            fillMode: Image.TileHorizontally
        }

        Image {
            anchors.top: parent.top
            anchors.left: parent.left
            source: "PanelGradient.png"
        }

        Image {
            id: backIcon
            source: "BackIcon.png"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 5

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                Connections {
                    function onClicked() {
                        nameText.editingFinished();
                        profilePage.back();
                    }
                }
            }
        }

        SmoothText {
            id: boxTitle
            anchors.left: backIcon.right
            anchors.top: parent.top
            anchors.leftMargin: 15
            anchors.topMargin: 5
            font.pixelSize: 64
            text: "Profile"
        }

        Item {
            anchors.left: parent.left
            anchors.leftMargin: 17
            anchors.right: parent.right
            anchors.rightMargin: 17
            anchors.top: backIcon.bottom

            SText {
                id: nameLabel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 16
                font.pixelSize: 16
                text: "Your Name:"
                color: "#ffffff"
            }

            Rectangle {
                id: nameRect
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.top: nameLabel.bottom
                anchors.topMargin: 10
                height: 30
                color: "#ffffff"
                clip: true

                TextInput {
                    id: nameText
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 13
                    font.family: duktofontsmall.name
                    focus: true
                    selectByMouse: true
                    text: guiBehind.buddyName
                    color: theme.mainColor
                    Connections {
                        function onEditingFinished() {
                            if (nameText.text !== "" && guiBehind.buddyName !== nameText.text) {
                                guiBehind.buddyName = nameText.text
                            }
                        }
                    }
                }
            }

            SText {
                id: labelAvatar
                anchors.left: parent.left
                anchors.top: nameRect.bottom
                anchors.topMargin: 10
                font.pixelSize: 16
                text: "Your Avatar:"
                color: "#ffffff"
            }

            Rectangle {
                color: theme.mainColor
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.top: labelAvatar.bottom
                anchors.topMargin: 10
                height: 68
                width: 68
                border.color: "#ffffff"
                border.width: 2

                Image {
                    anchors.left: parent.left
                    anchors.leftMargin: 2
                    anchors.top: parent.top
                    anchors.topMargin: 2
                    source: guiBehind.buddyAvatar
                    sourceSize.height: 64
                    sourceSize.width: 64
                    smooth: true
                    cache: false
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                    Connections {
                        function onClicked() {
                            guiBehind.selectAvatar();
                        }
                    }
                }
            }
        }

        ButtonDark {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: 10
            anchors.bottomMargin: 10
            buttonEnabled: true
            label: "Reset"
            Connections {
                function onClicked() {
                    guiBehind.resetBuddy();
                }
            }
        }
    }
}
