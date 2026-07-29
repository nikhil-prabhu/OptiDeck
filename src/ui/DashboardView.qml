import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: root
    title: i18nc("@title:page", "My Devices")

    padding: Kirigami.Units.gridUnit

    signal openWebcamDetail(var deviceData)

    signal openHidDetail(var deviceData)

    actions: Kirigami.Action
    {
        text: i18n("Refresh")
        icon.name: "view-refresh"
        onTriggered: deviceManager.refreshDevices()
    }

    function deviceIconSource(deviceType) {
        const typeIcons = {
            "keyboard": "input-keyboard",
            "mouse": "input-mouse",
            "tablet": "input-tablet",
            "receiver": "drive-removable-media",
            "headset": "audio-headset",
            "webcam": "camera-web"
        }
        return typeIcons[deviceType] || "preferences-desktop-peripherals"
    }

    function connectionIconSource(connectionType) {
        const typeIcons = {
            "bluetooth": "network-bluetooth-symbolic",
            "usb": "drive-removable-media-symbolic",
            "receiver": "network-wireless-hotspot-symbolic"
        }
        return typeIcons[connectionType] || "preferences-desktop-peripherals"
    }

    Flow {
        width: parent.width
        spacing: Kirigami.Units.gridUnit

        Repeater {
            model: deviceManager.devices

            delegate: Kirigami.AbstractCard
            {
                width: Kirigami.Units.gridUnit * 16
                height: Kirigami.Units.gridUnit * 14

                contentItem: Item {
                    anchors.fill: parent

                    ColumnLayout {
                        id: cardLayout
                        anchors.fill: parent
                        spacing: Kirigami.Units.largeSpacing

                        Kirigami.Icon {
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredWidth: Kirigami.Units.iconSizes.enormous
                            Layout.preferredHeight: Kirigami.Units.iconSizes.enormous
                            source: root.deviceIconSource(modelData.type)
                        }

                        Item {
                            Layout.fillHeight: true
                        }

                        Kirigami.Heading {
                            text: modelData.name
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            type: Kirigami.Heading.Type.Primary
                        }

                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: Kirigami.Units.largeSpacing

                            Kirigami.Badge {
                                icon.name: "battery-040" // Placeholder
                                text: modelData.battery + "%"
                                visible: modelData.battery !== undefined && modelData.battery >= 0

                            }

                            Kirigami.Badge {
                                icon.name: root.connectionIconSource(modelData.connectionType)
                                text: i18nc("@dashboard:connectionTypeBadge", modelData.connectionType.toUpperCase())
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                onClicked: {
                    const deviceData = {
                        deviceName: modelData.name,
                        devicePath: modelData.id,
                        deviceType: modelData.type,
                        controlsModel: modelData.controls || []
                    }

                    if (modelData.type === "webcam") {
                        root.openWebcamDetail(deviceData)
                    } else {
                        root.openHidDetail(deviceData)
                    }
                }
            }
        }
    }
}