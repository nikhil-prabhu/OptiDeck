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

        for (const auto &[id, name, type, minimum, maximum, step, defaultValue, currentValue, isInactive, menuItems]
             : cam.controls) {
            // Skip fixed or read-only controls
            if (minimum == maximum && menuItems.empty()) continue;

            QVariantMap ctrlMap;
            ctrlMap["id"] = id;
            ctrlMap["name"] = QString::fromStdString(name);
            ctrlMap["minimum"] = minimum;
            ctrlMap["maximum"] = maximum;
            ctrlMap["step"] = step;
            ctrlMap["defaultValue"] = defaultValue;
            ctrlMap["currentValue"] = currentValue;
            ctrlMap["isInactive"] = isInactive;

            QVariantList menuList;
            for (const auto &[index, name]: menuItems) {
                QVariantMap itemMap;
                itemMap["index"] = index;
                itemMap["name"] = QString::fromStdString(name);
                menuList.append(itemMap);
            }

            ctrlMap["menuItems"] = menuList;
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

QVariantList DeviceManager::getControlsForDevice(const QString &devicePath) {
    for (const auto cameras = V4L2Scanner::scanCameras(); const auto &cam: cameras) {
        if (QString::fromStdString(cam.devicePath) == devicePath) {
            QVariantList controlsList;
            for (const auto &[id, name, type, minimum, maximum, step, defaultValue, currentValue, isInactive, menuItems]
                 : cam.controls) {
                if (minimum == maximum && menuItems.empty()) continue;

                QVariantMap ctrlMap;
                ctrlMap["id"] = id;
                ctrlMap["name"] = QString::fromStdString(name);
                ctrlMap["type"] = type;
                ctrlMap["minimum"] = minimum;
                ctrlMap["maximum"] = maximum;
                ctrlMap["step"] = step;
                ctrlMap["defaultValue"] = defaultValue;
                ctrlMap["currentValue"] = currentValue;
                ctrlMap["isInactive"] = isInactive;

                QVariantList menuList;
                for (const auto &[index, name]: menuItems) {
                    QVariantMap itemMap;
                    itemMap["index"] = index;
                    itemMap["name"] = QString::fromStdString(name);
                    menuList.append(itemMap);
                }
                ctrlMap["menuItems"] = menuList;

                controlsList.append(ctrlMap);
            }
            return controlsList;
        }
    }
    return {};
}
