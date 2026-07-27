import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: qsTr("OptiDeck")

    Rectangle {
        anchors.fill: parent
        color: palette.window

        ScrollView {
            anchors.fill: parent
            anchors.margins: 20
            contentWidth: availableWidth

            Column {
                width: parent.width
                spacing: 24

                Text {
                    text: qsTr("Webcam Controls")
                    color: palette.windowText
                    font.pixelSize: 22
                    font.bold: true
                }

                Repeater {
                    model: cameraManager.cameras

                    delegate: Column {
                        id: cameraDelegate
                        required property var modelData

                        width: parent.width
                        spacing: 12

                        Rectangle {
                            width: parent.width
                            height: 40
                            color: palette.base
                            radius: 6

                            Text {
                                anchors.centerIn: parent
                                text: cameraDelegate.modelData.cardName + " (" + cameraDelegate.modelData.devicePath + ")"
                                color: palette.text
                                font.bold: true
                                font.pixelSize: 14
                            }
                        }

                        Repeater {
                            model: cameraDelegate.modelData.controls

                            delegate: Column {
                                id: controlDelegate
                                required property var modelData

                                width: parent.width
                                spacing: 4

                                Row {
                                    width: parent.width

                                    Text {
                                        text: controlDelegate.modelData.name
                                        color: palette.windowText
                                        font.pixelSize: 13
                                        width: 200
                                    }

                                    Text {
                                        text: slider.value
                                        color: palette.windowText
                                        font.pixelSize: 13
                                    }
                                }

                                Slider {
                                    id: slider
                                    width: parent.width
                                    from: controlDelegate.modelData.minimum
                                    to: controlDelegate.modelData.maximum
                                    stepSize: controlDelegate.modelData.step > 0 ? controlDelegate.modelData.step : 1
                                    value: controlDelegate.modelData.currentValue

                                    onMoved: {
                                        cameraManager.setControlValue(
                                            cameraDelegate.modelData.devicePath,
                                            controlDelegate.modelData.id,
                                            Math.round(value)
                                        )
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}