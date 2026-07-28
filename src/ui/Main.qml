import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Window {
    width: 1000
    height: 700
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

            Camera {
                id: camera
                active: true
            }

            CaptureSession {
                id: captureSession
                camera: camera
                videoOutput: videoOutput
            }

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
                        onClicked: {
                            camera.active = false
                            stackView.pop()
                        }
                    }

                    Image {
                        source: "image://theme/camera-web"
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

                // --- ROW 2: Two Columns (Live Video Feed & Controls Sliders) ---
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 20

                    // Row 2, Column 1: Live Webcam Feed
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: 3
                        color: "#000000"
                        radius: 8
                        clip: true

                        VideoOutput {
                            id: videoOutput
                            anchors.fill: parent
                            fillMode: VideoOutput.PreserveAspectFit
                        }
                    }

                    // Row 2, Column 2: Controls Panel
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: 2
                        clip: true

                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                        ListView {
                            id: controlsListView
                            width: parent.width
                            model: controlsModel
                            spacing: 16

                            delegate: ColumnLayout {
                                width: controlsListView.width - 20
                                spacing: 6

                                RowLayout {
                                    Layout.fillWidth: true
                                    Text {
                                        text: modelData.name
                                        font.pixelSize: 13
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                    }
                                    Text {
                                        text: {
                                            if (modelData.menuItems && modelData.menuItems.length > 0) {
                                                return comboBoxWidget.currentText
                                            } else if (modelData.minimum === 0 && modelData.maximum === 1) {
                                                return switchWidget.checked ? "1" : "0"
                                            } else {
                                                return sliderWidget.value
                                            }
                                        }
                                        font.pixelSize: 13
                                        font.bold: true
                                    }
                                }

                                Item {
                                    Layout.fillWidth: true
                                    height: {
                                        if (modelData.menuItems && modelData.menuItems.length > 0) return comboBoxWidget.height
                                        if (modelData.minimum === 0 && modelData.maximum === 1) return switchWidget.height
                                        return sliderWidget.height
                                    }

                                    // Dropdown ComboBox for Menu Controls (e.g. Auto Exposure)
                                    ComboBox {
                                        id: comboBoxWidget
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        visible: modelData.menuItems && modelData.menuItems.length > 0
                                        enabled: !modelData.isInactive
                                        opacity: enabled ? 1.0 : 0.4

                                        model: modelData.menuItems || []
                                        textRole: "name"
                                        valueRole: "index"

                                        Component.onCompleted: {
                                            if (modelData.menuItems) {
                                                for (var i = 0; i < modelData.menuItems.length; i++) {
                                                    if (modelData.menuItems[i].index === modelData.currentValue) {
                                                        currentIndex = i
                                                        break
                                                    }
                                                }
                                            }
                                        }

                                        onActivated: (index) => {
                                            var selectedValue = modelData.menuItems[index].index
                                            deviceManager.setWebcamControl(devicePath, modelData.id, selectedValue)
                                            // Refresh controls model to update dependent inactive states (e.g. unlock Exposure Time slider)
                                            controlsModel = deviceManager.getControlsForDevice(devicePath)
                                        }
                                    }

                                    // Binary Switch for Toggle Flags
                                    Switch {
                                        id: switchWidget
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        visible: (!modelData.menuItems || modelData.menuItems.length === 0) && (modelData.minimum === 0 && modelData.maximum === 1)
                                        enabled: !modelData.isInactive
                                        opacity: enabled ? 1.0 : 0.4
                                        checked: modelData.currentValue !== 0

                                        onToggled: {
                                            deviceManager.setWebcamControl(devicePath, modelData.id, checked ? 1 : 0)
                                            controlsModel = deviceManager.getControlsForDevice(devicePath)
                                        }
                                    }

                                    // Slider for Range Controls
                                    Slider {
                                        id: sliderWidget
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        visible: (!modelData.menuItems || modelData.menuItems.length === 0) && !(modelData.minimum === 0 && modelData.maximum === 1)
                                        enabled: !modelData.isInactive
                                        opacity: enabled ? 1.0 : 0.4
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
    }
}