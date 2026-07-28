#ifndef OPTIDECK_HIDSCANNER_H
#define OPTIDECK_HIDSCANNER_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct DiscoveredHidDevice {
    std::vector<std::string> devicePaths; // Holds all /dev/hidrawX nodes belonging to this device
    uint8_t deviceIndex{0};               // 0xFF for Receiver, 0x01..0x06 for paired devices
    uint16_t busType{0};
    uint16_t vendorId{0};
    uint16_t productId{0};
    std::string deviceName;
    bool isReceiver{false};
    int batteryPercentage{-1}; // -1 indicates unknown/unsupported
};

class HidScanner {
public:
    static constexpr uint16_t LOGITECH_VENDOR_ID = 0x046D;
    static std::vector<DiscoveredHidDevice> scanDevices();
};

#endif // OPTIDECK_HIDSCANNER_H