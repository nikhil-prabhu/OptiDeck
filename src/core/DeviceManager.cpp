#include "DeviceManager.h"
#include "V4L2Scanner.h"
#include "V4L2Controller.h"

DeviceManager::DeviceManager(QObject *parent) : QObject(parent) {
    refreshDevices();
}

QVariantList DeviceManager::devices() const {
    return m_devices;
}

void DeviceManager::refreshDevices() {
    QVariantList updatedList;

    // Scan Webcams via V4L2
    for (const auto cameras = V4L2Scanner::scanCameras(); const auto &cam: cameras) {
        QVariantMap dev;
        dev["id"] = QString::fromStdString(cam.devicePath);
        dev["name"] = QString::fromStdString(cam.cardName);
        dev["type"] = "webcam";
        dev["subtitle"] = QString::fromStdString(cam.devicePath);
        dev["isOnline"] = true;

        QVariantList controlsList;
        for (const auto &ctrl: cam.controls) {
            // Skip fixed or read-only controls
            if (ctrl.minimum == ctrl.maximum) continue;

            QVariantMap ctrlMap;
            ctrlMap["id"] = ctrl.id;
            ctrlMap["name"] = QString::fromStdString(ctrl.name);
            ctrlMap["minimum"] = ctrl.minimum;
            ctrlMap["maximum"] = ctrl.maximum;
            ctrlMap["step"] = ctrl.step;
            ctrlMap["defaultValue"] = ctrl.defaultValue;
            ctrlMap["currentValue"] = ctrl.currentValue;
            controlsList.append(ctrlMap);
        }
        dev["controls"] = controlsList;
        updatedList.append(dev);
    }

    m_devices = updatedList;
    emit devicesChanged();
}

bool DeviceManager::setWebcamControl(const QString &devicePath, const uint32_t controlId, const int value) {
    return V4L2Controller::setControl(devicePath.toStdString(), controlId, value);
}
