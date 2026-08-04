#include "CameraManager.h"
#include "v4l2/V4L2Controller.h"
#include "v4l2/V4L2Scanner.h"

#include <print>

CameraManager::CameraManager(QObject* parent) : QObject(parent) {
    refresh();
}

QVariantList CameraManager::cameras() const {
    return m_cameras;
}

void CameraManager::refresh() {
    m_cameras.clear();

    for (const auto& [devicePath, cardName, driverName, controls] : V4L2Scanner::scanCameras()) {
        QVariantMap camMap;
        camMap["id"] = QString::fromStdString(devicePath);
        camMap["name"] = QString::fromStdString(cardName);
        camMap["subtitle"] = QString::fromStdString(driverName);
        camMap["type"] = "webcam";
        camMap["isOnline"] = true;
        camMap["connectionType"] = "usb";
        camMap["battery"] = -1; // TODO: implement actual battery percentage extraction

        QVariantList controlsList;
        for (const auto& [id, name, type, minimum, maximum, step, defaultValue, currentValue, isInactive, menuItems] :
             controls) {
            // Filter out fixed/header controls unless menu items are present
            if (minimum == maximum && menuItems.empty())
                continue;

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
            for (const auto& [index, name] : menuItems) {
                QVariantMap itemMap;
                itemMap["index"] = index;
                itemMap["name"] = QString::fromStdString(name);
                menuList.append(itemMap);
            }
            ctrlMap["menuItems"] = menuList;

            controlsList.append(ctrlMap);
        }

        camMap["controls"] = controlsList;
        m_cameras.append(camMap);
    }

    emit camerasChanged();
}

bool CameraManager::setControlValue(const QString& devicePath, uint32_t controlId, int value) {
    const bool success = V4L2Controller::setControl(devicePath.toStdString(), controlId, value);
    if (success) {
        std::println("[OptiDeck] Updated control {:#010x} on {} to {}", controlId, devicePath.toStdString(), value);
    }
    return success;
}

QVariantList CameraManager::getControlsForDevice(const QString& devicePath) {
    refresh(); // Fetch updated states/flags from hardware

    for (const QVariant& camVar : m_cameras) {
        if (const QVariantMap cam = camVar.toMap(); cam["id"].toString() == devicePath) {
            return cam["controls"].toList();
        }
    }

    return {};
}
