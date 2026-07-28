import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property string deviceName: ""
    property string devicePath: ""
    property string deviceType: ""
    property var controlsModel: []

    signal goBack()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        // --- ROW 1: Header ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Button {
                text: qsTr("← Back")
                onClicked: goBack()
            }

            Image {
                source: ((type) => {
                    const typeIcons = {
                        "keyboard": "image://theme/input-keyboard",
                        "mouse": "image://theme/input-mouse",
                        "tablet": "image://theme/input-tablet",
                        "receiver": "image://theme/drive-removable-media",
                        "headset": "image://theme/audio-headset",
                    };
                    return typeIcons[type] || "image://theme/preferences-desktop-peripherals";
                })(deviceType)
                sourceSize.width: 32
                sourceSize.height: 32
                fillMode: Image.PreserveAspectFit
            }

            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true

                Text {
                    text: deviceName
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: devicePath
                    font.pixelSize: 11
                    color: palette.text
                    opacity: 0.5
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: palette.mid
        }

        // --- Placeholder Body ---
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                anchors.centerIn: parent
                text: qsTr("Hardware controls coming soon.")
                font.pixelSize: 16
                color: palette.text
                opacity: 0.5
            }
        }
    }
}