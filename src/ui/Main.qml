import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 900
    height: 600
    visible: true
    title: qsTr("OptiDeck")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: dashboardView
    }

    // --- DASHBOARD PAGE ---
    Component {
        id: dashboardView

        Item {
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 32
                spacing: 24

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: qsTr("My Devices")
                        font.pixelSize: 24
                        font.bold: true
                        Layout.fillWidth: true
                    }
                    Button {
                        text: qsTr("Refresh")
                        onClicked: deviceManager.refreshDevices()
                    }
                }

                // Horizontal layout for detected devices
                Row {
                    Layout.fillWidth: true
                    spacing: 20

                    Repeater {
                        model: deviceManager.devices
                        delegate: Item {
                            width: 180
                            height: 220

                            Rectangle {
                                anchors.fill: parent
                                color: mouseArea.containsMouse ? palette.alternateBase : palette.base
                                border.color: palette.mid
                                border.width: 1
                                radius: 8

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 16
                                    spacing: 8

                                    // TODO: dynamically set icon based on device type
                                    Image {
                                        Layout.alignment: Qt.AlignHCenter
                                        source: "image://theme/camera-web"
                                        sourceSize.width: 64
                                        sourceSize.height: 64
                                        fillMode: Image.PreserveAspectFit
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }

                                    Text {
                                        text: modelData.name
                                        font.bold: true
                                        font.pixelSize: 14
                                        horizontalAlignment: Text.AlignHCenter
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        text: qsTr("Battery: --") // Placeholder
                                        font.pixelSize: 11
                                        color: palette.text
                                        opacity: 0.6
                                        horizontalAlignment: Text.AlignHCenter
                                        Layout.fillWidth: true
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }
                                }

                                MouseArea {
                                    id: mouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        stackView.push(deviceDetailView, {
                                            deviceName: modelData.name,
                                            devicePath: modelData.id,
                                            controlsModel: modelData.controls || []
                                        })
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    // --- DEVICE DETAIL PAGE ---
    Component {
        id: deviceDetailView

        Item {
            property string deviceName: ""
            property string devicePath: ""
            property var controlsModel: []

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 32
                spacing: 20

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Button {
                        text: qsTr("← Back")
                        onClicked: stackView.pop()
                    }

                    Text {
                        text: deviceName
                        font.pixelSize: 22
                        font.bold: true
                        Layout.fillWidth: true
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        width: parent.width
                        model: controlsModel
                        spacing: 16

                        delegate: ColumnLayout {
                            width: parent.width
                            spacing: 6

                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: modelData.name
                                    font.pixelSize: 14
                                    Layout.fillWidth: true
                                }
                                Text {
                                    text: slider.value
                                    font.pixelSize: 14
                                    font.bold: true
                                }
                            }

                            Slider {
                                id: slider
                                Layout.fillWidth: true
                                from: modelData.minimum
                                to: modelData.maximum
                                stepSize: modelData.step > 0 ? modelData.step : 1
                                value: modelData.currentValue

                                onMoved: {
                                    deviceManager.setWebcamControl(devicePath, modelData.id, value)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}