#include "DeviceManager.h"

DeviceManager::DeviceManager(QObject *parent) : QObject(parent) {
    refreshDevices();
}

QVariantList DeviceManager::devices() const {
    return m_devices;
}

void DeviceManager::refreshDevices() {
    QVariantList updatedList;

    // Refresh & map webcams via CameraManager
    m_cameraManager.refresh();
    for (const auto &camVar: m_cameraManager.cameras()) {
        QVariantMap cam = camVar.toMap();

        QVariantMap dev;
        dev["id"] = cam["devicePath"];
        dev["name"] = cam["cardName"];
        dev["type"] = "webcam";
        dev["subtitle"] = cam["devicePath"];
        dev["isOnline"] = true;
        dev["controls"] = cam["controls"];

        updatedList.append(dev);
    }

    // TODO: Future expansion: Map HID++ devices via HidManager
    // for (const auto &hidVar : m_hidManager.devices()) { ... }

    m_devices = updatedList;
    emit devicesChanged();
}

bool DeviceManager::setWebcamControl(const QString &devicePath, const uint32_t controlId, const int value) {
    return CameraManager::setControlValue(devicePath, controlId, value);
}

QVariantList DeviceManager::getControlsForDevice(const QString &devicePath) {
    return m_cameraManager.getControlsForDevice(devicePath);
}
