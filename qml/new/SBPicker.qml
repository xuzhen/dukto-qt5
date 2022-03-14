//  Saturation/brightness picking box
import QtQuick 2.3

Item {
    id: root
    property color hueColor : "blue"
    property real saturation : NaN
    property real brightness : NaN

    signal changed()

    function setValue(sat, brigh) {
        pickerCursor.x = sat * width;
        pickerCursor.y = (1 - brigh) * height;

        root.saturation = sat
        root.brightness = brigh
        root.changed();
    }

    Connections {
        function onHeightChanged() {
            if (!isNaN(root.saturation)) {
                setValue(root.saturation, root.brightness)
            }
        }
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
    function handleMouse(mouse) {
        if (mouse.buttons & Qt.LeftButton) {
            pickerCursor.x = Math.max(0, Math.min(width,  mouse.x));
            pickerCursor.y = Math.max(0, Math.min(height, mouse.y));

            root.saturation = pickerCursor.x/width
            root.brightness = 1 - pickerCursor.y/height
            root.changed();
        }
    }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: containsMouse ? (pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor) : Qt.ArrowCursor
        Connections {
            function onPositionChanged(mouse) {
                handleMouse(mouse)
            }
            function onPressed(mouse) {
                handleMouse(mouse)
            }
        }
    }
}

