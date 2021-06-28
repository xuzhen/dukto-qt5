//  Saturation/brightness picking box
import QtQuick 2.3

Item {
    id: root
    property color hueColor : "blue"
    property real saturation : pickerCursor.x/width
    property real brightness : 1 - pickerCursor.y/height

    signal changed()

    function setValue(sat, brigh) {
        pickerCursor.x = sat * width;
        pickerCursor.y = (1 - brigh) * height;
    }

    // width: 126; height: 126
    clip: true
    Rectangle {
        anchors.fill: parent;
        rotation: -90
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFFFFF" }
            GradientStop { position: 1.0; color: root.hueColor }
        }
        border.color: "#f0f0f0"
        border.width: 2
    }
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 1.0; color: "#FF000000" }
            GradientStop { position: 0.0; color: "#00000000" }
        }
    }
    Item {
        id: pickerCursor
        property int r : 8
        Rectangle {
            x: -parent.r; y: -parent.r
            width: parent.r*2; height: parent.r*2
            radius: parent.r
            border.color: "black"; border.width: 2
            color: "transparent"
            Rectangle {
                anchors.fill: parent; anchors.margins: 2;
                border.color: "white"; border.width: 2
                radius: width/2
                color: "transparent"
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        function handleMouse(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                pickerCursor.x = Math.max(0, Math.min(width,  mouse.x));
                pickerCursor.y = Math.max(0, Math.min(height, mouse.y));
                root.changed();
            }
        }
        onPositionChanged: handleMouse(mouse)
        onPressed: handleMouse(mouse)
    }
}

