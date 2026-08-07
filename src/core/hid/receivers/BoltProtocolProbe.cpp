#include "BoltProtocolProbe.h"
#include "../../HidppProtocol.h"
#include "../HidppDiagnostics.h"

#include <algorithm>
#include <print>

using namespace HidppProtocol;

std::optional<std::string> BoltProtocolProbe::identifyReceiver(const uint16_t productId) const {
    // Known Bolt receiver PID(s).
    if (productId == 0xC548)
        return "Bolt Receiver";
    return std::nullopt;
}

std::optional<HidppReceiver::PairingInfo> BoltProtocolProbe::getPairingInfo(
    HidppReceiver& hidpp, const uint8_t deviceIndex) const {
    if (deviceIndex < 1 || deviceIndex > 6)
        return std::nullopt;

    const auto subAddress = static_cast<uint8_t>(0x51 + (deviceIndex - 1) * 0x10);
    const auto res = hidpp.readLongRegister(REG_PAIRING_INFORMATION, subAddress);
    if (!res.success || res.data.size() < 4) {
        std::println("[OptiDeck] BoltProtocolProbe({}): sub-address 0x{:02x} -- {}",
                     deviceIndex, subAddress, hidppDescribeFailure(res));
        return std::nullopt;
    }

    const bool allZeroPastEcho = std::all_of(res.data.begin() + 1, res.data.end(),
                                             [](const uint8_t b) { return b == 0; });
    if (allZeroPastEcho) {
        std::println("[OptiDeck] BoltProtocolProbe({}): sub-address 0x{:02x} answered but slot is empty",
                     deviceIndex, subAddress);
        return std::nullopt;
    }

    HidppReceiver::PairingInfo info;
    info.wirelessProductId = static_cast<uint16_t>(res.data[2] | (res.data[3] << 8));
    info.deviceKind = res.data[1] & 0x0F;
    std::println("[OptiDeck] BoltProtocolProbe({}): HIT at sub-address 0x{:02x} -> wpid=0x{:04x}, kind={}",
                 deviceIndex, subAddress, info.wirelessProductId, info.deviceKind);
    return info;
}

std::optional<std::string> BoltProtocolProbe::getDeviceName(
    HidppReceiver& hidpp, const uint8_t deviceIndex) const {
    if (deviceIndex < 1 || deviceIndex > 6)
        return std::nullopt;

    const auto subAddress = static_cast<uint8_t>(0x61 + (deviceIndex - 1) * 0x10);
    const auto res = hidpp.readLongRegister(REG_PAIRING_INFORMATION, subAddress, 0x01);
    if (!res.success || res.data.size() < 3) {
        std::println("[OptiDeck] BoltProtocolProbe({}): name sub-address 0x{:02x} -- {}",
                     deviceIndex, subAddress, hidppDescribeFailure(res));
        return std::nullopt;
    }

    const uint8_t length = res.data[2];
    if (length == 0 || static_cast<size_t>(3 + length) > res.data.size()) {
        std::println("[OptiDeck] BoltProtocolProbe({}): name sub-address 0x{:02x} returned an implausible length ({})",
                     deviceIndex, subAddress, length);
        return std::nullopt;
    }

    std::string devName(res.data.begin() + 3, res.data.begin() + 3 + length);
    std::println("[OptiDeck] BoltProtocolProbe({}): name sub-address 0x{:02x} -> \"{}\"",
                 deviceIndex, subAddress, devName);
    return devName;
}