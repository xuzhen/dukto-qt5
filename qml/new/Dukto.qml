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
    id: mainElement

    FontLoader {
        id: duktofont
        source: "Klill-Light.ttf"
    }

    FontLoader {
        id: duktofontsmall
        source: "LiberationSans-Regular.ttf"
    }

    FontLoader {
        id: duktofonthappy
        source: "KGLikeASkyscraper.ttf"
    }

    Connections {
         target: guiBehind
         function onTransferStart() {
             duktoOverlay.state = "progress"
         }
         function onReceiveCompleted() {
             duktoOverlay.state = ""
             duktoInner.gotoPage("recent");
         }
         function onGotoTextSnippet() {
             duktoOverlay.state = "showtext"
         }
         function onGotoSendPage() {
             duktoOverlay.state = "send";
         }
         function onGotoMessagePage() {
             duktoOverlay.state = "message";
         }
         function onGotoProfilePage() {
             duktoOverlay.state = "profile";
         }
         function onHideAllOverlays() {
             duktoOverlay.state = "";
         }
    }

    Rectangle {
        id: duktoTopPad
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: guiBehind.screenPadding.top
    }

    Rectangle {
        id: duktoBottomPad
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: guiBehind.screenPadding.bottom
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: duktoTopPad.bottom
        anchors.bottom: duktoBottomPad.top

        Rectangle {
            id: duktoLeftPad
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: guiBehind.screenPadding.left
        }

        Rectangle {
            id: duktoRightPad
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: guiBehind.screenPadding.right
        }

        DuktoInner {
            id: duktoInner
            anchors.left: duktoLeftPad.right
            anchors.right: duktoRightPad.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            Connections {
                function onShowIpList() {
                    duktoOverlay.state = "ip"
                }
                function onShowSettings() {
                    duktoOverlay.refreshSettingsColor();
                    duktoOverlay.state = "settings";
                }
            }
        }

        UpdatesBox {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 100
        }

        DuktoOverlay {
            id: duktoOverlay
            anchors.fill: duktoInner
        }
    }

    Binding {
        target: guiBehind
        property: "overlayState"
        value: duktoOverlay.state
    }
}
