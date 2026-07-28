#include "CameraManager.h"
#include "V4L2Scanner.h"
#include "V4L2Controller.h"

#include <print>

CameraManager::CameraManager(QObject *parent) : QObject(parent) {
    refresh();
}

QVariantList CameraManager::cameras() const {
    return m_cameras;
}

void CameraManager::refresh() {
    m_cameras.clear();

    for (const auto &[devicePath, cardName, driverName, controls]: V4L2Scanner::scanCameras()) {
        QVariantMap camMap;
        camMap["devicePath"] = QString::fromStdString(devicePath);
        camMap["cardName"] = QString::fromStdString(cardName);
        camMap["driverName"] = QString::fromStdString(driverName);

        QVariantList controlsList;
        for (const auto &[id, name, type, minimum, maximum, step, defaultValue, currentValue, isInactive, menuItems]:
             controls) {
            // Filter out menu/header controls with 0-range unless menu items are present
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

        camMap["controls"] = controlsList;
        m_cameras.append(camMap);
    }

    emit camerasChanged();
}

bool CameraManager::setControlValue(const QString &devicePath, uint32_t controlId, int value) {
    const bool success = V4L2Controller::setControl(devicePath.toStdString(), controlId, value);
    if (success) {
        std::println("[OptiDeck] Updated control {:#010x} on {} to {}", controlId, devicePath.toStdString(), value);
    }
    return success;
}
