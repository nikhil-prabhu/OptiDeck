import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    width: 800
    height: 550
    visible: true
    title: qsTr("OptiDeck")

    Rectangle {
        anchors.fill: parent
        color: palette.window

        Column {
            anchors.centerIn: parent
            spacing: 16

            Text {
                text: qsTr("OptiDeck")
                color: palette.windowText
                font.pixelSize: 22
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}