#include "HidManager.h"
#include <linux/input.h>
#include <QVariantMap>
#include "../LogitechIds.h"

using namespace LogitechIds;

HidManager::HidManager(QObject* parent) : QObject(parent) {}

void HidManager::refresh() {
    m_devices.clear();
    for (const auto& hidDev : HidScanner::scanDevices()) {
        QVariantMap dev;

        // The path alone is only unique for the receiver itself and for
        // standalone (direct USB/BT) devices. Devices paired to a receiver
        // all share the receiver's devicePath (comms are routed through
        // it), so append deviceIndex to keep those unique.
        const QString basePath = QString::fromStdString(hidDev.devicePaths.front());
        dev["id"] = hidDev.isReceiver ? basePath : QString("%1:%2").arg(basePath).arg(hidDev.deviceIndex);
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
    case Receiver::BOLT:
        return "receiver";

    case Mouse::MX_MASTER_3S_BOLT:
    case Mouse::MX_MASTER_3S_MAC:
    case Mouse::MX_MASTER_3:
    case Mouse::M196:
        return "mouse";

    case Keyboard::MX_KEYS:
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
