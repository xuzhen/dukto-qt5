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

Flipable {
   id: contactDelegateItem
   width: parent == null ? 0 : parent.width
   height: buddyIp == "" ? 94 : 64

   property string buddyIp
   property alias buddyGeneric: buddyGenericImage.source
   property alias buddyAvatar: buddyAvatarImage.source
   property alias buddyOsLogo: buddyOsLogoImage.source
   property alias buddyUsername: buddyUsernameText.text
   property alias buddySystem: buddySystemText.text
   property bool buddyShowBack: false

   MouseArea {
       id: buddyMouseArea
       anchors.fill: parent
       hoverEnabled: true
       cursorShape: (buddyIp && buddyIp != "-" && containsMouse) ? Qt.PointingHandCursor : Qt.ArrowCursor
       Connections {
           function onClicked() {
               guiBehind.showSendPage(buddyIp)
           }
       }
   }

   Rectangle {
       anchors.fill: parent
       color: "#00000000"
       visible: buddyIp && buddyIp != "-" && buddyMouseArea.containsMouse;
       Rectangle {
           anchors.right: parent.right
           anchors.top: parent.top
           height: 64
           width: 5
           color: theme.color3
       }
   }

   front: Item {
       id: temp
       width: contactDelegateItem.width

       Flipable {
           id: flipableAvatar
           width: 64
           height: 64

           front: Rectangle {
               anchors.fill: parent
               color: theme.color2
               Image {
                   anchors.fill: parent
                   source: "TileGradient.png"
               }
               Image {
                   id: buddyGenericImage
                   anchors.fill: parent
               }
               Image {
                   id: buddyAvatarImage
                   anchors.fill: parent
                   smooth: true
               }
           }

           back: Rectangle {
               anchors.fill: parent
               color: theme.color2
               Image {
                   anchors.fill: parent
                   source: "TileGradient.png"
               }
               Image {
                   id: buddyOsLogoImage
                   anchors.fill: parent
               }
           }

           transform: Rotation {
                id: innerRotation
                origin.x: 32
                origin.y: 32
                axis.x: 1; axis.y: 0; axis.z: 0
                angle: 0
           }

           states: [
               State {
                name: "OsState"
                when: buddyShowBack
           }]

           transitions: [
                Transition {
                    from: ""
                    to: "OsState"
                    SequentialAnimation {
                        NumberAnimation { target: innerRotation; property: "angle"; from: 0; to: 180; duration: 400; easing.type: Easing.OutQuad }
                        PauseAnimation { duration: 3000 }
                        NumberAnimation { target: innerRotation; property: "angle"; from: 180; to: 0; duration: 400; easing.type: Easing.OutQuad }
                    }
                }
           ]
       }
       SText {
           id: buddyUsernameText
           anchors.top:  flipableAvatar.top
           anchors.topMargin: 8
           anchors.left: flipableAvatar.right
           anchors.leftMargin: 10
           anchors.right: parent.right
           anchors.rightMargin: 20
           font.pixelSize: 16
           elide: Text.ElideRight
           color: "#555555"
       }
       SText {
           id: buddySystemText
           anchors.left: flipableAvatar.right
           anchors.leftMargin: 10
           anchors.top: buddyUsernameText.bottom
           anchors.topMargin: 5
           anchors.right: parent.right
           anchors.rightMargin: 20
           font.pixelSize: 14
           elide: Text.ElideRight
           color: "#555555"
       }
   }

   transform: Rotation {
        id: rotation
        origin.x: 32
        origin.y: 32
        axis.x: 1; axis.y: 0; axis.z: 0     // set axis.y to 1 to rotate around y-axis
        angle: 0    // the default angle
   }

   NumberAnimation { id: addAnimation; target: rotation; property: "angle"; from: -90; to: 0; duration: 300; easing.type: Easing.OutCubic }
   SequentialAnimation {
       id: removeAnimation
       PropertyAction { target: contactDelegateItem; property: "ListView.delayRemove"; value: true }
       NumberAnimation { target: rotation; property: "angle"; from: 0; to: -90; duration: 300; easing.type: Easing.InCubic }
       PropertyAction { target: contactDelegateItem; property: "ListView.delayRemove"; value: false }
   }
   ListView.onAdd: addAnimation.start()
   ListView.onRemove: removeAnimation.start()

   Rectangle {
       color: theme.color2
       x: 40
       width: parent.width - 80
       height: 1
       y: parent.height - 10
       visible: buddyIp == ""
   }
}
