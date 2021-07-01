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
import QtQuick.Controls 2.0

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
            onClicked: settingsPage.back();
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
            horizontalAlignment: "AlignLeft"
            verticalAlignment: "AlignVCenter"
            elide: "ElideMiddle"
            font.pixelSize: 12
            text: guiBehind.currentPath
        }
    }

    ButtonDark {
        id: buttonPath
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: textPath.bottom
        anchors.topMargin: 10
        label: "Change folder"
        onClicked: guiBehind.changeDestinationFolder()
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
        onChanged: {
            guiBehind.changeThemeColor(colorValue);
        }
    }

    ColorBox {
        id: cbox1
        anchors.top: labelColor.bottom
        anchors.topMargin: 8
        anchors.left: picker.right
        anchors.leftMargin: 20
        color: "#248B00"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox2
        anchors.top: cbox1.top
        anchors.left: cbox1.right
        anchors.leftMargin: 15
        color: "#A80000"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox3
        anchors.top: cbox1.top
        anchors.left: cbox2.right
        anchors.leftMargin: 15
        color: "#3A6CBC"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox4
        anchors.top: cbox1.bottom
        anchors.topMargin: 15
        anchors.left: cbox1.left
        color: "#2e3436" // "#704214"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox5
        anchors.left: cbox4.right
        anchors.top: cbox4.top
        anchors.leftMargin: 15
        color: "#B77994"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox6
        anchors.top: cbox4.top
        anchors.left: cbox5.right
        anchors.leftMargin: 15
        color: "#5B2F42"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox7
        anchors.top: cbox4.bottom
        anchors.topMargin: 15
        anchors.left: cbox4.left
        color: "#353B56"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox8
        anchors.top: cbox7.top
        anchors.left: cbox7.right
        anchors.leftMargin: 15
        color: "#FB8504"
        onClicked: picker.setColor(color)
    }

    ColorBox {
        id: cbox9
        anchors.top: cbox7.top
        anchors.left: cbox8.right
        anchors.leftMargin: 15
        color: "#6D0D71"
        onClicked: picker.setColor(color)
    }


    CheckBox {
        id: nswitch
        anchors.top: cbox7.bottom
        anchors.left: labelPath.left
        anchors.topMargin: 30
        checked: guiBehind.showNotification
        hoverEnabled: false
        indicator: Rectangle {
            implicitWidth: 16
            implicitHeight: 16
            border.color: "#888888"
            border.width: 1
            Rectangle {
                visible: nswitch.checked
                color: theme.color2
                anchors.margins: 3
                anchors.fill: parent
            }
        }
        contentItem: SText {
            font.pixelSize: 16
            text: "Enable Notification"
            color: "#888888"
            anchors.top: nswitch.top
            anchors.left: nswitch.indicator.right
            anchors.leftMargin: 4
            anchors.topMargin: -1
        }
        onClicked: guiBehind.showNotification = nswitch.checked
    }
}
