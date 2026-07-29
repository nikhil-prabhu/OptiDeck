import QtQuick
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: root
    property string deviceName: ""
    property string devicePath: ""
    property string deviceType: ""
    property var controlsModel: []

    title: deviceName

    function deviceIconSource(type) {
        const typeIcons = {
            "keyboard": "image://theme/input-keyboard",
            "mouse": "image://theme/input-mouse",
            "tablet": "image://theme/input-tablet",
            "receiver": "image://theme/drive-removable-media",
            "headset": "image://theme/audio-headset"
        }
        return typeIcons[type] || "image://theme/preferences-desktop-peripherals"
    }

    // --- Placeholder Body ---
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        icon.source: root.deviceIconSource(root.deviceType)
        text: i18n("Hardware controls coming soon.")
        explanation: root.devicePath
    }
}
