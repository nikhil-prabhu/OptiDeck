#include "HidScanner.h"
#include "HidppReceiver.h"
#include "receivers/ReceiverProbeRegistry.h"

#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <sstream>

std::vector<DiscoveredHidDevice> HidScanner::scanDevices() {
    std::vector<DiscoveredHidDevice> finalDevices;
    std::map<uint16_t, DiscoveredHidDevice> receiverGroups;
    const ReceiverProbeRegistry probeRegistry;

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
                            // The raw USB descriptor name is often generic
                            // (e.g. this project's own Bolt receiver reports
                            // itself simply as "Logitech USB Receiver") and
                            // doesn't reliably say which family it belongs
                            // to; prefer a friendly name if a probe
                            // recognizes the PID, falling back to the raw
                            // name otherwise.
                            dev.deviceName = probeRegistry.identifyReceiver(static_cast<uint16_t>(product))
                                                 .value_or(hidName.empty() ? "Unknown Logitech Device" : hidName);
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

    // Phase 2: query each receiver via the HID++ 1.0 register-access
    // protocol (see HidppReceiver.h) rather than kernel-exposed sysfs child
    // nodes, which don't reliably appear for every receiver/driver
    // combination. A receiver can expose more than one hidraw interface; we
    // try each until one answers.
    std::vector<DiscoveredHidDevice> pairedDevices;
    for (auto& dev : finalDevices) {
        if (!dev.isReceiver)
            continue;

        bool queried = false;
        for (const auto& path : dev.devicePaths) {
            HidppReceiver hidpp(path);
            if (!hidpp.isOpen())
                continue;

            const auto count = hidpp.getConnectedDeviceCount();
            if (!count) {
                std::println("[OptiDeck] {} did not answer register 0x02 (connection state)", path);
                continue;
            }
            std::println("[OptiDeck] {} reports {} connected device(s)", path, *count);
            queried = true;

            if (*count == 0)
                break; // receiver confirmed empty, nothing more to do here

            for (uint8_t deviceIndex = 1; deviceIndex <= 6; ++deviceIndex) {
                auto probed = probeRegistry.probeDevice(hidpp, deviceIndex);
                if (!probed)
                    continue;

                std::println("[OptiDeck] device {} identified via {} protocol: \"{}\" (wpid=0x{:04x})",
                             deviceIndex, probed->protocolFamily, probed->deviceName,
                             probed->pairingInfo.wirelessProductId);

                DiscoveredHidDevice child;
                child.devicePaths.push_back(path);
                child.deviceIndex = deviceIndex;
                child.busType = 0; // routed via receiver, not a direct USB/BT bus of its own
                child.vendorId = LOGITECH_VENDOR_ID;
                child.productId = probed->pairingInfo.wirelessProductId;
                child.deviceName = probed->deviceName;
                child.isReceiver = false;
                child.batteryPercentage = -1;
                pairedDevices.push_back(child);
            }

            break; // this receiver interface answered; no need to try its others
        }

        if (!queried) {
            std::println("[OptiDeck] Could not get a register reply from receiver PID {:#06x} on any of its {} interface(s)",
                         dev.productId, dev.devicePaths.size());
        }
    }

    for (auto& child : pairedDevices) {
        finalDevices.push_back(child);
    }

    for (auto& dev : finalDevices) {
        if (!dev.isReceiver) {
            dev.batteryPercentage = -1; // TODO: implement actual battery percentage retrieval
        }
    }

    return finalDevices;
}