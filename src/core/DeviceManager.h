#pragma once

#include <QVariantList>
#include <QVariantMap>

class DeviceManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList devices READ devices NOTIFY devicesChanged)

public:
    explicit DeviceManager(QObject *parent = nullptr);

    [[nodiscard]] QVariantList devices() const;

    Q_INVOKABLE void refreshDevices();

    Q_INVOKABLE static bool setWebcamControl(const QString &devicePath, uint32_t controlId, int value);

signals:
    void devicesChanged();

private:
    QVariantList m_devices;
};
