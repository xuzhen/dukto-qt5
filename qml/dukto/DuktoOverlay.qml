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
    state: guiBehind.showTermsOnStart ? "termspage" : ""

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
        }
    }

    IpPage {
        id: ipPage
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        width: parent.width
        x: -50
        opacity: 0
        onBack: parent.state = ""
    }

    ProgressPage {
        id: progressPage
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        width: parent.width
        x: -50
        opacity: 0
    }

    ShowTextPage {
        id: showTextPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
        x: -50
        opacity: 0
        onBack: parent.state = ""
        onBackOnSend: {
            sendPage.setDestinationFocus();
            parent.state = "send"
        }
    }

    SettingsPage {
        id: settingsPage
        width: parent.width
        height: parent.height
        x: -50
        opacity: 0
        onBack: parent.state = ""
    }

    SendPage {
        id: sendPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height
        x: -50
        opacity: 0
        onBack: parent.state = ""
        onShowTextPage: {
            showTextPage.setTextEditFocus();
            parent.state = "showtext";
        }
    }

    MessagePage {
        id: messagePage
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        width: parent.width
        x: -50
        opacity: 0
        onBack: parent.state = backState
    }

    TermsPage {
        id: termsPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
        x: -50
        opacity: 0
        onOk: {
            guiBehind.showTermsOnStart = false;
            parent.state = ""
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
        }
    ]

    transitions: [
        Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutCubic; duration: 500 }
            NumberAnimation { properties: "opacity"; easing.type: Easing.OutCubic; duration: 500 }
        }
    ]
}
