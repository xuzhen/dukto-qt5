//  Color manipulation utilities

//  creates color value from hue, saturation, brightness, alpha
function hsba(h, s, b, a) {

    var lightness = (2 - s)*b;
    var satHSL = s*b/((lightness <= 1) ? lightness : 2 - lightness);
    lightness /= 2;

    return Qt.hsla(h, satHSL, lightness, a);
}

//  creates a full color string from color value and alpha[0..1], e.g. "#FF00FF00"
function fullColorString(clr, a) {
    return "#" + ((Math.ceil(a*255) + 256).toString(16).substr(1, 2) +
            clr.toString().substr(1, 6)).toUpperCase();
}

//  extracts integer color channel value [0..255] from color value
function getChannelStr(clr, channelIdx) {
    return parseInt(clr.toString().substr(channelIdx*2 + 1, 2), 16);
}
