//  A toy QML colorpicker control, by Ruslan Shestopalyuk
import QtQuick 2.3
import "ColorUtils.js" as ColorUtils

Rectangle {
    id: colorPicker
    property color colorValue: ColorUtils.hsba(hueSlider.value, sbPicker.saturation,
                                               sbPicker.brightness, 1)
    color: "#FFFFFF"

    signal changed()

    function setColor(color) {

        var h = theme.getHue(color);
        var s = theme.getSaturation(color);
        var b = theme.getLightness(color);

        hueSlider.setValue(h);
        sbPicker.setValue(s, b);

        this.changed();
    }

    Row {
        anchors.fill: parent
        spacing: 3

        // saturation/brightness picker box
        SBPicker {
            id: sbPicker
            hueColor : ColorUtils.hsba(hueSlider.value, 1.0, 1.0, 1.0, 1.0)
            width: parent.height; height: parent.height
            onChanged: {
                colorPicker.changed();
            }
        }

        // hue picking slider
        Item {
            width: 12; height: parent.height
            Rectangle {
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop { position: 1.0;  color: "#FF0000" }
                    GradientStop { position: 0.85; color: "#FFFF00" }
                    GradientStop { position: 0.76; color: "#00FF00" }
                    GradientStop { position: 0.5;  color: "#00FFFF" }
                    GradientStop { position: 0.33; color: "#0000FF" }
                    GradientStop { position: 0.16; color: "#FF00FF" }
                    GradientStop { position: 0.0;  color: "#FF0000" }
                }
                border.color: "#f0f0f0"
                border.width: 2
            }
            ColorSlider {
                id: hueSlider
                anchors.fill: parent
                onChanged: colorPicker.changed()
            }
        }
    }
}
