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
    id: termsPage
    color: "#ffffff"

    signal ok()

    MouseArea {
        anchors.fill: parent
    }

    SmoothText {
        id: boxTitle
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 15
        anchors.topMargin: 5
        font.pixelSize: 64
        text: "Disclaimer"
        color: theme.lighterColor
    }

    SText {
        id: boxSender
        anchors.left: boxTitle.left
        anchors.top: parent.top
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.topMargin: 45
        font.pixelSize: 16
        text: "Please read and press 'accept'"
        color: "#888888"
    }


    Rectangle {
        id: rectangleText
        border.color: theme.lighterColor
        border.width: 1
        anchors.top: boxSender.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.bottom: buttonCopy.top
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 10

        Flickable {
            id: flickableText
            anchors.fill: parent
            anchors.margins: 5
            contentHeight: textEditSnippet.paintedHeight
            flickableDirection: Flickable.VerticalFlick

            clip: true

            function ensureVisible(r) {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX+width <= r.x+r.width)
                    contentX = r.x+r.width-width;
                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY+height <= r.y+r.height)
                    contentY = r.y+r.height-height;
            }

            TextEdit {
                id: textEditSnippet
                color: "#888888"
                width: rectangleText.width - 10
                font.family: duktofontsmall.name
                font.pixelSize: 13
                selectByMouse: true
                wrapMode: TextEdit.Wrap
                textFormat: TextEdit.PlainText
                readOnly: true
                text: "The Software is provided on an AS IS basis, without warranty of any kind, including without limitation the warranties of merchantability, fitness for a particular purpose and non-infringement. The entire risk as to the quality and performance of the Software is borne by you. Should the Software prove defective, you and not the author assume the entire cost of any service and repair.\n\nTHE AUTHOR IS NOT RESPONSIBLE FOR ANY INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES OF ANY CHARACTER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF GOODWILL, WORK STOPPAGE, COMPUTER FAILURE OR MALFUNCTION, OR ANY AND ALL OTHER COMMERCIAL DAMAGES OR LOSSES.\n\nTitle, ownership rights and intellectual property rights in and to the Software shall remain to the author.\n\nAlso, the Software will periodically send anonymous data to the author servers for statistical information, like (but not only) the Software version, operating system version, application settings, current locale."
                onCursorRectangleChanged: flickableText.ensureVisible(cursorRectangle)
            }
        }
    }

    ButtonDark {
        id: buttonCopy
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        buttonEnabled: true
        label: "Accept"
        onClicked: termsPage.ok();
    }
}
