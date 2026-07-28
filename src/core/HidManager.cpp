#include "HidManager.h"
#include <linux/input.h>
#include <QVariantMap>

HidManager::HidManager(QObject* parent) : QObject(parent) {}

void HidManager::refresh() {
    m_devices.clear();
    for (const auto& hidDev : HidScanner::scanDevices()) {
        QVariantMap dev;

        // Just use the first path as the unique UI identifier for this device tile
        // TODO: properly identify the actual path later
        dev["id"] = QString::fromStdString(hidDev.devicePaths.front());
        dev["name"] = QString::fromStdString(hidDev.deviceName);
        dev["subtitle"] = QString("PID: 0x%1").arg(hidDev.productId, 4, 16, QChar('0')).toUpper();
        dev["isOnline"] = true;
        dev["type"] = determineDeviceType(hidDev.productId, dev["name"].toString(), hidDev.isReceiver);

        if (hidDev.busType == BUS_BLUETOOTH) {
            dev["connectionType"] = "bluetooth";
        } else if (hidDev.busType == BUS_USB) {
            dev["connectionType"] = "usb";
        } else {
            dev["connectionType"] = "receiver"; // Future: Devices paired to a dongle
        }

        dev["controls"] = QVariantList();
        dev["battery"] = hidDev.batteryPercentage;

        m_devices.append(dev);
    }
}

QString HidManager::determineDeviceType(const uint16_t productId, const QString& deviceName, const bool isReceiver) {
    // TODO: properly map out these PIDs elsewhere, and add more products
    switch (productId) {
    case 0xC548: // Logi Bolt Receiver
        return "receiver";
    case 0xB034: // MX Master 3S (Bluetooth/Bolt)
    case 0xB035: // MX Master 3S (Mac Edition)
    case 0xB023: // MX Master 3
    case 0xB03F: // M196
        return "mouse";
    case 0xB353: // MX Keys
        return "keyboard";
    default:
        break;
    }

    // Fallback heuristic if PID isn't mapped yet
    const QString lowerName = deviceName.toLower();
    if (lowerName.contains("mouse") || lowerName.contains("mx"))
        return "mouse";
    if (lowerName.contains("keyboard") || lowerName.contains("keys"))
        return "keyboard";

    return "unknown";
}

QVariantList HidManager::devices() const {
    return m_devices;
}