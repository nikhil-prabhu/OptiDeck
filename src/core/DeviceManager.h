#pragma once

#include "CameraManager.h"

class DeviceManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList devices READ devices NOTIFY devicesChanged)

public:
    explicit DeviceManager(QObject* parent = nullptr);

    [[nodiscard]] QVariantList devices() const;

    Q_INVOKABLE void refreshDevices();

    Q_INVOKABLE static bool setWebcamControl(const QString& devicePath, uint32_t controlId, int value);

    Q_INVOKABLE QVariantList getControlsForDevice(const QString& devicePath);

signals:
    void devicesChanged();

private:
    CameraManager m_cameraManager;
    QVariantList m_devices;
};
