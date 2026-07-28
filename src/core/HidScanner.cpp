#include "HidScanner.h"
#include <fstream>
#include <map>
#include <ranges>
#include <sstream>

std::vector<DiscoveredHidDevice> HidScanner::scanDevices() {
    std::vector<DiscoveredHidDevice> finalDevices;
    std::map<uint16_t, DiscoveredHidDevice> receiverGroups;

    const fs::path hidrawPath{"/sys/class/hidraw"};
    if (!fs::exists(hidrawPath))
        return finalDevices;

    for (const auto& entry : fs::directory_iterator(hidrawPath)) {
        std::string devName = entry.path().filename().string();
        if (!devName.starts_with("hidraw"))
            continue;

        fs::path ueventPath = entry.path() / "device" / "uevent";
        if (!fs::exists(ueventPath))
            continue;

        std::ifstream ueventFile(ueventPath);
        std::string line, hidId, hidName;

        while (std::getline(ueventFile, line)) {
            if (line.starts_with("HID_ID="))
                hidId = line.substr(7);
            else if (line.starts_with("HID_NAME="))
                hidName = line.substr(9);
        }

        std::stringstream ss(hidId);
        std::string vendorStr, productStr;

        if (std::string busStr; std::getline(ss, busStr, ':') &&
                                std::getline(ss, vendorStr, ':') &&
                                std::getline(ss, productStr, ':')) {
            try {
                unsigned int product = 0;
                unsigned int vendor = 0;
                unsigned int bus = 0;
                bus = std::stoul(busStr, nullptr, 16);
                vendor = static_cast<unsigned int>(std::stoul(vendorStr, nullptr, 16));
                product = static_cast<unsigned int>(std::stoul(productStr, nullptr, 16));

                if (vendor == LOGITECH_VENDOR_ID) {
                    std::string fullPath = "/dev/" + devName;
                    bool isReceiver = (hidName.find("Receiver") != std::string::npos ||
                                       hidName.find("Bolt") != std::string::npos);

                    if (isReceiver) {
                        if (receiverGroups.contains(product)) {
                            receiverGroups[product].devicePaths.push_back(fullPath);
                        } else {
                            DiscoveredHidDevice dev;
                            dev.devicePaths.push_back(fullPath);
                            dev.busType = bus;
                            dev.vendorId = static_cast<uint16_t>(vendor);
                            dev.productId = static_cast<uint16_t>(product);
                            dev.deviceName = hidName.empty() ? "Unknown Logitech Device" : hidName;
                            dev.isReceiver = true;
                            receiverGroups[product] = dev;
                        }
                    } else {
                        DiscoveredHidDevice dev;
                        dev.devicePaths.push_back(fullPath);
                        dev.busType = bus;
                        dev.vendorId = static_cast<uint16_t>(vendor);
                        dev.productId = static_cast<uint16_t>(product);
                        dev.deviceName = hidName.empty() ? "Unknown Logitech Device" : hidName;
                        dev.isReceiver = false;
                        finalDevices.push_back(dev);
                    }
                }
            } catch (const std::exception&) {
                // Handle conversion error silently if uevent formatting is invalid
            }
        }
    }

    for (const auto& dev : receiverGroups | std::views::values) {
        finalDevices.push_back(dev);
    }

    for (auto& dev : finalDevices) {
        if (!dev.isReceiver) {
            dev.batteryPercentage = -1; // TODO: implement actual battery percentage retrieval
        }
    }

    return finalDevices;
}