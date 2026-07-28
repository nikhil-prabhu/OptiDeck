#ifndef OPTIDECK_HIDMANAGER_H
#define OPTIDECK_HIDMANAGER_H

#include <QString>
#include <QVariantList>
#include "HidScanner.h"

class HidManager : public QObject {
    Q_OBJECT
public:
    explicit HidManager(QObject* parent = nullptr);
    void refresh();
    [[nodiscard]] QVariantList devices() const;

private:
    static QString determineDeviceType(uint16_t productId, const QString& deviceName, bool isReceiver);
    QVariantList m_devices;
};

#endif // OPTIDECK_HIDMANAGER_H