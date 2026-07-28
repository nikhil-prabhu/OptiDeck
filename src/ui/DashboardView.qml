import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    signal openWebcamDetail(var deviceData)

    signal openHidDetail(var deviceData)

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

        // --- Horizontal Scrolling Carousel ---
        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 300 // Expanded height to fit larger cards
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AsNeeded
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff

            RowLayout {
                spacing: 20

                Repeater {
                    model: deviceManager.devices
                    delegate: Item {
                        width: 280
                        height: 300

                        Rectangle {
                            anchors.fill: parent
                            color: mouseArea.containsMouse ? palette.alternateBase : palette.base
                            border.color: palette.mid
                            border.width: 1
                            radius: 12

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 12

                                Image {
                                    source: ((deviceType) => {
                                        const typeIcons = {
                                            "keyboard": "image://theme/input-keyboard",
                                            "mouse": "image://theme/input-mouse",
                                            "tablet": "image://theme/input-tablet",
                                            "receiver": "image://theme/drive-removable-media",
                                            "headset": "image://theme/audio-headset",
                                            "webcam": "image://theme/camera-web"
                                        };
                                        return typeIcons[deviceType] || "image://theme/preferences-desktop-peripherals";
                                    })(modelData.type)

                                    Layout.alignment: Qt.AlignHCenter
                                    sourceSize.width: 96
                                    sourceSize.height: 96
                                    fillMode: Image.PreserveAspectFit
                                }

                                Item {
                                    Layout.fillHeight: true
                                }

                                Text {
                                    text: modelData.name
                                    font.bold: true
                                    font.pixelSize: 15
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignHCenter
                                    spacing: 12

                                    Item {
                                        Layout.fillWidth: true
                                    }

                                    Image {
                                        source: ((connectionType) => {
                                            const typeIcons = {
                                                "bluetooth": "image://theme/network-bluetooth-symbolic",
                                                "usb": "image://theme/drive-removable-media-symbolic",
                                                "receiver": "image://theme/network-wireless-hotspot-symbolic",
                                            };
                                            return typeIcons[connectionType] || "image://theme/preferences-desktop-peripherals";
                                        })(modelData.connectionType)
                                        sourceSize.width: 32
                                        sourceSize.height: 32
                                        fillMode: Image.PreserveAspectFit
                                    }

                                    RowLayout {
                                        visible: modelData.battery !== undefined && modelData.battery >= 0
                                        spacing: 4

                                        Image {
                                            source: "image://theme/battery"
                                            sourceSize.width: 14
                                            sourceSize.height: 14
                                            fillMode: Image.PreserveAspectFit
                                        }
                                        Text {
                                            text: modelData.battery + "%"
                                            font.pixelSize: 12
                                            color: palette.text
                                        }
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }
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
                                    var deviceData = {
                                        deviceName: modelData.name,
                                        devicePath: modelData.id,
                                        deviceType: modelData.type,
                                        controlsModel: modelData.controls || []
                                    }

                                    if (modelData.type === "webcam") {
                                        openWebcamDetail(deviceData)
                                    } else {
                                        openHidDetail(deviceData)
                                    }
                                }
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