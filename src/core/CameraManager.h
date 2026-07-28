#ifndef OPTIDECK_CAMERAMANAGER_H
#define OPTIDECK_CAMERAMANAGER_H

#include <QVariantMap>
#include <QString>

class CameraManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList cameras READ cameras NOTIFY camerasChanged)

public:
    explicit CameraManager(QObject *parent = nullptr);

    [[nodiscard]] QVariantList cameras() const;

    Q_INVOKABLE void refresh();

    Q_INVOKABLE static bool setControlValue(const QString &devicePath, uint32_t controlId, int value);

    Q_INVOKABLE QVariantList getControlsForDevice(const QString &devicePath);

signals:
    void camerasChanged();

private:
    QVariantList m_cameras;
};

#endif // OPTIDECK_CAMERAMANAGER_H
