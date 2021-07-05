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
    id: showTextPage
    color: "#ffffff"

    signal back()
    signal backOnSend()

    function setTextEditFocus() {
        textEditSnippet.focus = true;
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
                    if (guiBehind.textSnippetSending)
                        showTextPage.backOnSend();
                    else
                        showTextPage.back();
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
        text: "Text snippet"
        color: theme.color3
    }

    SText {
        id: boxSender
        anchors.left: backIcon.right
        anchors.top: parent.top
        anchors.leftMargin: 17
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.topMargin: 45
        elide: "ElideRight"
        font.pixelSize: 16
        text: (guiBehind.textSnippetSending ? "to " : "from ") + guiBehind.textSnippetBuddy
        color: "#888888"
    }


    Rectangle {
        id: rectangleText
        border.color: theme.color3
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
                color: "#555555"
                width: rectangleText.width - 10
                font.family: duktofontsmall.name
                font.pixelSize: 13
                focus: true
                selectByMouse: true
                wrapMode: TextEdit.Wrap
                textFormat: TextEdit.PlainText
                readOnly: guiBehind.textSnippetSending ? false : true
                text: guiBehind.textSnippet
                Connections {
                    function onCursorRectangleChanged() {
                        flickableText.ensureVisible(textEditSnippet.cursorRectangle)
                    }
                }
            }
        }
    }

    ButtonDark {
        id: buttonCopy
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        buttonEnabled: guiBehind.textSnippetSending ? guiBehind.clipboardTextAvailable : true
        label: guiBehind.textSnippetSending ? "Paste from clipboard" : "Copy to clipboard"
        Connections {
            function onClicked() {
                if (guiBehind.textSnippetSending)
                    textEditSnippet.paste();
                else {
                    if (textEditSnippet.selectedText == "")
                        textEditSnippet.selectAll();
                    textEditSnippet.copy();
                }
            }
        }
    }

    ButtonDark {
        id: buttonSend
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 15
        anchors.bottomMargin: 10
        label: "Send"
        visible: guiBehind.textSnippetSending
        buttonEnabled: textEditSnippet.text != ""
        Connections {
            function onClicked() {
                guiBehind.textSnippet = textEditSnippet.text;
                guiBehind.sendText();
            }
        }
    }
}
