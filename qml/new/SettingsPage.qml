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

Rectangle {
    id: settingsPage
    color: "#ffffff"

    signal back()

    function refreshColor() {

        picker.setColor(theme.color2);
    }

    MouseArea {
        anchors.fill: parent
    }

    Image {
        id: backIcon
        source: "BackIconDark.png"
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
                    settingsPage.back();
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
        text: "Settings"
        color: theme.color3
    }

    SText {
        id: labelPath
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 17
        anchors.topMargin: 70
        font.pixelSize: 16
        text: "Save received file in:"
        color: "#888888"
    }

    Rectangle {
        id: textPath
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: labelPath.bottom
        anchors.leftMargin: 17
        anchors.rightMargin: 17
        anchors.topMargin: 8
        height: 30
        color: theme.color2
        clip: true

        Image {
            anchors.top: parent.top
            anchors.left: parent.left
            source: "PanelGradient.png"
        }

        SText {
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.fill: parent
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideMiddle
            font.pixelSize: 12
            text: guiBehind.destPath
        }
    }

    ButtonDark {
        id: buttonPath
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: textPath.bottom
        anchors.topMargin: 10
        label: "Change folder"
        Connections {
            function onClicked() {
                guiBehind.changeDestinationFolder()
            }
        }
    }

    SText {
        id: labelColor
        anchors.left: labelPath.left
        anchors.top: buttonPath.bottom
        anchors.topMargin: 30
        font.pixelSize: 16
        text: "Theme color:"
        color: "#888888"
    }

    ColorPicker {
        id: picker
        anchors.top: labelColor.bottom
        anchors.topMargin: 8
        anchors.left: labelColor.left
        width: Math.min(parent.width * 0.4, 200)
        height: width
        Connections {
            function onChanged() {
                guiBehind.changeThemeColor(picker.colorValue);
            }
        }
    }

    Item {
        id: colorBoxes
        anchors.top: labelColor.bottom
        anchors.topMargin: 8
        anchors.bottom: picker.bottom
        anchors.left: picker.right
        anchors.leftMargin: picker.width / 4
        anchors.right: parent.right
        anchors.rightMargin: 17

        ColorBox {
            id: cbox1
            anchors.top: parent.top
            anchors.left: parent.left
            color: "#248B00"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox2
            anchors.top: cbox1.top
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#A80000"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox3
            anchors.top: cbox1.top
            anchors.right: parent.right
            color: "#3A6CBC"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox4
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: cbox1.left
            color: "#2e3436" // "#704214"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox5
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            color: "#B77994"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox6
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            color: "#5B2F42"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox7
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            color: "#353B56"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox8
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#FB8504"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }

        ColorBox {
            id: cbox9
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            color: "#6D0D71"
            width: picker.width / 4
            Connections {
                function onClicked(color) {
                    picker.setColor(color)
                }
            }
        }
    }

    CheckBox {
        id: nswitch
        visible: guiBehind.isDesktopApp()
        anchors.top: picker.bottom
        anchors.left: labelPath.left
        anchors.topMargin: 30
        text: "Enable Notification"
        checked: guiBehind.showNotification

        Connections {
            function onClicked(checked) {
                guiBehind.showNotification = checked
            }
        }
    }

    CheckBox {
        id: cswitch
        visible: guiBehind.isDesktopApp()
        anchors.top: nswitch.bottom
        anchors.left: labelPath.left
        anchors.topMargin: 25
        text: "Minimize to system tray on close"
        checked: guiBehind.closeToTray

        Connections {
            function onClicked(checked) {
                guiBehind.closeToTray = checked
            }
        }
    }
}
