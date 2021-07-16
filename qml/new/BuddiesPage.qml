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
    clip: true

    SText {
        id: refreshLabel
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        color: "#888888"
        wrapMode: Text.WordWrap
        text: "Refresh Buddies"
        horizontalAlignment: Text.AlignHCenter
        visible: buddiesList.draggingVertically && buddiesList.contentY < -(height + 20)

        Connections {
            function onVisibleChanged() {
                // ListView's onFlickStarted event is not reliable
                if (!buddiesList.draggingVertically && buddiesList.contentY < -(refreshLabel.height + 20)) {
                    guiBehind.refreshNeighbors();
                }
            }
        }
    }

    ListView {
        id: buddiesList
        anchors.fill: parent
        spacing: 10
        anchors.leftMargin: 25
        anchors.rightMargin: 0
        model: buddiesListData // EsempioDati {}

        Component {
             id: contactDelegate
             BuddyListElement {
                 buddyIp: ip
                 buddyAvatar: avatar
                 buddyGeneric: generic
                 buddyUsername: username
                 buddySystem: system
                 buddyOsLogo: oslogo
                 buddyShowBack: showback
             }
         }

        delegate: contactDelegate
    }
}
