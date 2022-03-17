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
    color: "#00000000"
    state: guiBehind.showTermsOnStart ? "termspage" : (guiBehind.initError !== "" ? "initerr" : "")

    function refreshSettingsColor() {

        settingsPage.refreshColor();
    }

    Rectangle {
        id: disabler
        anchors.fill: parent
        color: "#ccffffff"
        opacity: 0
        visible: false

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    IpPage {
        id: ipPage
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        width: parent.width
        x: -parent.width
        opacity: 0
        Connections {
            function onBack() {
                state = ""
            }
        }
    }

    ProgressPage {
        id: progressPage
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        width: parent.width
        x: -parent.width
        opacity: 0
    }

    ShowTextPage {
        id: showTextPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
        x: -parent.width
        opacity: 0
        Connections {
            function onBack() {
                state = ""
            }
            function onBackOnSend() {
                sendPage.setDestinationFocus();
                state = "send"
            }
        }
    }

    SettingsPage {
        id: settingsPage
        width: parent.width
        height: parent.height
        x: -parent.width
        opacity: 0
        Connections {
            function onBack() {
                state = ""
            }
        }
    }

    SendPage {
        id: sendPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height
        x: -parent.width
        opacity: 0
        Connections {
            function onBack() {
                state = ""
            }
            function onShowTextPage() {
                showTextPage.setTextEditFocus();
                state = "showtext";
            }
        }
    }

    MessagePage {
        id: messagePage
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        width: parent.width
        x: -parent.width
        opacity: 0
        Connections {
            function onBack(backState) {
                state = backState
            }
        }
    }

    TermsPage {
        id: termsPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
        x: -parent.width
        opacity: 0
        Connections {
            function onOk() {
                guiBehind.showTermsOnStart = false;
            }
        }
    }

    InitErrPage {
        id: initErrPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
        x: -parent.width
        opacity: 0
        Connections {
            function onAction(actionName) {
                guiBehind.reinitialize(actionName);
            }
        }
    }

    ProfilePage {
        id: profilePage
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        width: parent.width
        x: -parent.width
        opacity: 0
        Connections {
            function onBack() {
                state = ""
            }
        }
    }

    states: [
        State {
            name: "ip"
            PropertyChanges {
                target: ipPage
                opacity: 1
                x: 0
            }
            PropertyChanges {
                target: disabler
                opacity: 1
                visible: true
            }
        },
        State {
            name: "progress"
            PropertyChanges {
                target: progressPage
                opacity: 1
                x: 0
            }
            PropertyChanges {
                target: disabler
                opacity: 1
                visible: true
            }
        },
        State {
            name: "showtext"
            PropertyChanges {
                target: showTextPage
                opacity: 1
                x: 0
            }
        },
        State {
            name: "settings"
            PropertyChanges {
                target: settingsPage
                opacity: 1
                x: 0
            }
        },
        State {
            name: "send"
            PropertyChanges {
                target: sendPage
                opacity: 1
                x: 0
            }
        },
        State {
            name: "message"
            PropertyChanges {
                target: messagePage
                opacity: 1
                x: 0
            }
            PropertyChanges {
                target: disabler
                opacity: 1
                visible: true
            }
        },
        State {
            name: "termspage"
            PropertyChanges {
                target: termsPage
                opacity: 1
                x: 0
            }
        },
        State {
            name: "initerr"
            PropertyChanges {
                target: initErrPage
                opacity: 1
                x: 0
            }
            PropertyChanges {
                target: disabler
                opacity: 1
                visible: true
            }
        },
        State {
            name: "profile"
            PropertyChanges {
                target: profilePage
                opacity: 1
                x: 0
            }
            PropertyChanges {
                target: disabler
                opacity: 1
                visible: true
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutCubic; duration: 500 }
            NumberAnimation { properties: "opacity"; easing.type: Easing.OutCubic; duration: 500 }
        }
    ]
}
