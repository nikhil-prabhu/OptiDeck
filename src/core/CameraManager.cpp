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

    for (auto scannedCameras = V4L2Scanner::scanCameras(); const auto& [devicePath, cardName, driverName, controls] : scannedCameras) {
        QVariantMap camMap;
        camMap["devicePath"] = QString::fromStdString(devicePath);
        camMap["cardName"] = QString::fromStdString(cardName);
        camMap["driverName"] = QString::fromStdString(driverName);

        QVariantList controlsList;
        for (const auto& [id, name, minimum, maximum, step, defaultValue, currentValue] : controls) {
            // Filter out menu/header controls with 0-range
            if (minimum == maximum) continue;

            QVariantMap ctrlMap;
            ctrlMap["id"] = id;
            ctrlMap["name"] = QString::fromStdString(name);
            ctrlMap["minimum"] = minimum;
            ctrlMap["maximum"] = maximum;
            ctrlMap["step"] = step;
            ctrlMap["defaultValue"] = defaultValue;
            ctrlMap["currentValue"] = currentValue;

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