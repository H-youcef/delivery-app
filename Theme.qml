pragma Singleton

import QtQuick 2.13
import QtQuick.Window 2.13

QtObject {
//    readonly property color gray: "#b2b1b1"
//    readonly property color lightGray: "#dddddd"
//    readonly property color light: "#ffffff"
//    readonly property color blue: "#2d548b"
//    property color mainColor: "#17a81a"
//    readonly property color dark: "#222222"
//    readonly property color mainColorDarker: Qt.darker(mainColor, 1.5)

    property real windowWidth
    property real windowHeight

    readonly property real baseTextSize:  12
    readonly property real tinyTextSize: 8
    readonly property real smallTextSize: 10
    readonly property real largeTextSize: 16

    readonly property int baseMarginSize:  dp(30)
    readonly property int tinyMarginSize: dp(8)
    readonly property int smallMarginSize: dp(15)
    readonly property int largeMarginSize: dp(60)
    readonly property int hugeMarginSize:  dp(80)


    readonly property real baseIconSize: dp(65)
    readonly property real smalIconSize: dp(50)
    readonly property real largeIconSize: dp(100)

//    property font font
//    font.bold: true
//    font.underline: false
//    font.pixelSize: 14
//    font.family: "arial"

    function dp(number) {
        return Math.round(number * Screen.pixelDensity / 10)
    }

}
