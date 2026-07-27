#ifndef OPTIDECK_HIDSCANNER_H
#define OPTIDECK_HIDSCANNER_H

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>

namespace fs = std::filesystem;

struct DiscoveredHidDevice {
    std::string devicePath;
    uint8_t deviceIndex{0};   // 0xFF for Receiver, 0x01..0x06 for paired devices
    uint16_t vendorId{0};
    uint16_t productId{0};
    std::string deviceName;
    bool isReceiver{false};
};

class HidScanner {
public:
    static constexpr uint16_t LOGITECH_VENDOR_ID = 0x046D;

    // Scans `/sys/class/hidraw` and queries HID++ receivers for paired devices
    static std::vector<DiscoveredHidDevice> scanDevices();
};

#endif // OPTIDECK_HIDSCANNER_H