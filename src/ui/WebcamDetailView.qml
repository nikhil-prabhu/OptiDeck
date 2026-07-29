import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import QtMultimedia
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: root
    property string deviceName: ""
    property string devicePath: ""
    property string deviceType: ""
    property var controlsModel: []

    title: deviceName
    padding: Kirigami.Units.largeSpacing

    Component.onDestruction: camera.active = false

    MediaDevices {
        id: mediaDevices
    }

    // Evaluate the matching index once to avoid property binding loops
    property int deviceIndex: {
        const inputs = mediaDevices.videoInputs;
        for (let i = 0; i < inputs.length; ++i) {
            if (String(inputs[i].id) === root.devicePath) {
                return i;
            }
        }
        return -1;
    }

    Camera {
        id: camera
        // Only attempt to start if the exact device is found
        active: root.deviceIndex !== -1

        // Pass the matched device if found; otherwise assign the default just to satisfy the QML type requirement
        cameraDevice: root.deviceIndex !== -1 ? mediaDevices.videoInputs[root.deviceIndex] : mediaDevices.defaultVideoInput
    }

    CaptureSession {
        id: captureSession
        camera: camera
        videoOutput: videoOutput
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        Controls.Label {
            Layout.fillWidth: true
            visible: root.devicePath.length > 0
            text: root.devicePath
            opacity: 0.6
            font.pixelSize: Kirigami.Theme.smallFont.pixelSize
            elide: Text.ElideRight
        }

        // --- Live Video Feed & Controls ---
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Kirigami.Units.largeSpacing

            // Column 1: Live Webcam Feed
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 3
                color: "#000000"
                radius: Kirigami.Units.cornerRadius
                clip: true

                VideoOutput {
                    id: videoOutput
                    anchors.fill: parent
                    fillMode: VideoOutput.PreserveAspectFit
                    // Hide the output layer if there is an error or no match
                    visible: root.deviceIndex !== -1 && camera.error === Camera.NoError
                }

                // Error Overlay
                ColumnLayout {
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.largeSpacing * 2
                    visible: !videoOutput.visible
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        Layout.alignment: Qt.AlignHCenter
                        source: "camera-off"
                        implicitWidth: Kirigami.Units.iconSizes.huge
                        implicitHeight: Kirigami.Units.iconSizes.huge
                        color: "#ffffff" // Force white so it's visible on the black background
                        isMask: true
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                        font.bold: true
                        color: "#ffffff"
                        text: root.deviceIndex === -1 ? i18n("Camera Not Found") : i18n("Unable to Start Feed")
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                        color: "#aaaaaa"
                        text: root.deviceIndex === -1
                            ? i18n("The multimedia engine could not locate a feed for %1.", root.devicePath)
                            : camera.errorString
                    }
                }
            }

            // Column 2: Controls Panel
            Controls.ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 2
                clip: true

                Controls.ScrollBar.vertical.policy: Controls.ScrollBar.AlwaysOn

                ListView {
                    id: controlsListView
                    width: parent.width
                    model: root.controlsModel
                    spacing: Kirigami.Units.gridUnit * 2.5

                    delegate: ColumnLayout {
                        width: controlsListView.width - Kirigami.Units.largeSpacing
                        spacing: Kirigami.Units.smallSpacing
                        opacity: modelData.isInactive ? 0.5 : 1.0

                        RowLayout {
                            Layout.fillWidth: true
                            Controls.Label {
                                text: modelData.name
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                            Controls.Label {
                                text: {
                                    if (modelData.menuItems && modelData.menuItems.length > 0) {
                                        return comboBoxWidget.currentText
                                    } else if (modelData.minimum === 0 && modelData.maximum === 1) {
                                        return switchWidget.checked ? "ON" : "OFF"
                                    } else {
                                        const name = modelData.name ? modelData.name.toLowerCase() : "";
                                        const val = sliderLoader.item ? sliderLoader.item.value : modelData.currentValue;

                                        if (name.indexOf("temperature") !== -1 || name.indexOf("white balance") !== -1) {
                                            return val + " K"
                                        } else if (name.indexOf("exposure") !== -1 || name.indexOf("time") !== -1) {
                                            return val + " ms"
                                        } else if (name.indexOf("gain") !== -1 || name.indexOf("pan") !== -1 || name.indexOf("tilt") !== -1) {
                                            return val
                                        } else {
                                            const min = modelData.minimum;
                                            const max = modelData.maximum;
                                            if (max === min) return "0%"
                                            const percent = Math.round(((val - min) / (max - min)) * 100);
                                            return percent + "%"
                                        }
                                    }
                                }
                                font.bold: true
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            height: {
                                if (modelData.menuItems && modelData.menuItems.length > 0) return comboBoxWidget.height
                                if (modelData.minimum === 0 && modelData.maximum === 1) return switchWidget.height
                                return sliderLoader.item ? sliderLoader.item.height : Kirigami.Units.gridUnit * 2
                            }

                            // Dropdown ComboBox for Menu Controls
                            Controls.ComboBox {
                                id: comboBoxWidget
                                anchors.left: parent.left
                                anchors.right: parent.right
                                visible: modelData.menuItems && modelData.menuItems.length > 0
                                enabled: !modelData.isInactive

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
                                    deviceManager.setWebcamControl(root.devicePath, modelData.id, selectedValue)
                                    Qt.callLater(() => {
                                        root.controlsModel = deviceManager.getControlsForDevice(root.devicePath)
                                    })
                                }
                            }

                            // Binary Switch for Toggle Flags
                            Controls.Switch {
                                id: switchWidget
                                anchors.left: parent.left
                                anchors.right: parent.right
                                visible: (!modelData.menuItems || modelData.menuItems.length === 0) && (modelData.minimum === 0 && modelData.maximum === 1)
                                enabled: !modelData.isInactive
                                checked: modelData.currentValue !== 0

                                onToggled: {
                                    deviceManager.setWebcamControl(root.devicePath, modelData.id, checked ? 1 : 0)
                                    Qt.callLater(() => {
                                        root.controlsModel = deviceManager.getControlsForDevice(root.devicePath)
                                    })
                                }
                            }

                            // Loader for Sliders
                            Loader {
                                id: sliderLoader
                                anchors.left: parent.left
                                anchors.right: parent.right
                                visible: (!modelData.menuItems || modelData.menuItems.length === 0) && !(modelData.minimum === 0 && modelData.maximum === 1)

                                property bool isTemp: modelData.name && (modelData.name.toLowerCase().indexOf("temperature") !== -1 || modelData.name.toLowerCase().indexOf("white balance") !== -1)

                                sourceComponent: isTemp ? tempSliderComp : standardSliderComp
                            }

                            // Dedicated Temperature Slider Component
                            Component {
                                id: tempSliderComp

                                Controls.Slider {
                                    id: tempWidget
                                    width: parent.width
                                    enabled: !modelData.isInactive
                                    from: modelData.minimum
                                    to: modelData.maximum
                                    stepSize: modelData.step > 0 ? modelData.step : 1
                                    value: modelData.currentValue

                                    onMoved: {
                                        deviceManager.setWebcamControl(root.devicePath, modelData.id, value)
                                    }

                                    onPressedChanged: {
                                        if (!pressed) {
                                            Qt.callLater(() => {
                                                root.controlsModel = deviceManager.getControlsForDevice(root.devicePath)
                                            })
                                        }
                                    }

                                    background: Rectangle {
                                        x: tempWidget.leftPadding
                                        y: tempWidget.topPadding + tempWidget.availableHeight / 2 - height / 2
                                        width: tempWidget.availableWidth
                                        height: 18
                                        radius: 3
                                        color: "transparent"

                                        gradient: Gradient {
                                            orientation: Gradient.Horizontal
                                            GradientStop {
                                                position: 0.0; color: "#4da6ff"
                                            }
                                            GradientStop {
                                                position: 0.5; color: "#e3e3e3"
                                            }
                                            GradientStop {
                                                position: 1.0; color: "#ffb84d"
                                            }
                                        }

                                        Rectangle {
                                            width: tempWidget.visualPosition * parent.width
                                            height: parent.height
                                            color: "transparent"
                                            radius: parent.radius
                                        }
                                    }
                                }
                            }

                            // Standard Native Slider Component
                            Component {
                                id: standardSliderComp

                                Controls.Slider {
                                    width: parent.width
                                    enabled: !modelData.isInactive
                                    from: modelData.minimum
                                    to: modelData.maximum
                                    stepSize: modelData.step > 0 ? modelData.step : 1
                                    value: modelData.currentValue

                                    onMoved: {
                                        deviceManager.setWebcamControl(root.devicePath, modelData.id, value)
                                    }

                                    onPressedChanged: {
                                        if (!pressed) {
                                            Qt.callLater(() => {
                                                root.controlsModel = deviceManager.getControlsForDevice(root.devicePath)
                                            })
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
