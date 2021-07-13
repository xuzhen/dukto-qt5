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
    state: "buddies"
    color: "#ffffff"

    signal showIpList()
    signal showSettings()

    function gotoPage(page) {
        tabBar.state = page;
        state = page;
    }

    TabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: parent.state = tab
    }

    ToolBar {
        id: toolBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: {
            if (command === "ip") showIpList();
            if (command === "settings") showSettings();
        }
    }

    BuddiesPage {
        id: buddiesPage
        anchors.top: tabBar.bottom
        anchors.topMargin: 10
        anchors.bottom: toolBar.top
        width: parent.width
        x: 200
        opacity: 0
    }

    RecentPage {
        id: recentPage
        anchors.top: tabBar.bottom
        anchors.topMargin: 10
        anchors.bottom: toolBar.top
        width: parent.width
        x: parent.width
        opacity: 0
    }

    AboutPage {
        id: aboutPage
        anchors.top: tabBar.bottom
        anchors.topMargin: 10
        anchors.bottom: toolBar.top
        width: parent.width
        x: parent.width
        opacity: 0
    }

    states: [
        State {
            name: "buddies"

            PropertyChanges {
                target: buddiesPage
                opacity: 1
                x: 0
            }
        },
        State {
            name: "recent"
            PropertyChanges {
                target: buddiesPage
                opacity: 0
                x: -buddiesPage.width
            }
            PropertyChanges {
                target: recentPage
                opacity: 1
                x: 0
            }
        },
        State {
            name: "about"

            PropertyChanges {
                target: buddiesPage
                opacity: 0
                x: -buddiesPage.width
            }
            PropertyChanges {
                target: recentPage
                opacity: 0
                x: -recentPage.width
            }
            PropertyChanges {
                target: aboutPage
                opacity: 1
                x: 0
            }

        }
    ]

    transitions: [
        Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuart; duration: 1000 }
            NumberAnimation { properties: "opacity"; easing.type: Easing.InQuad; duration: 500 }
        }
    ]
}
