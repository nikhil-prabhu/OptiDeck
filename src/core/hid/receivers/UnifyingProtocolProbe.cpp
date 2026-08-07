#include "UnifyingProtocolProbe.h"
#include "../../HidppProtocol.h"
#include "../HidppDiagnostics.h"

#include <print>

using namespace HidppProtocol;

std::optional<std::string> UnifyingProtocolProbe::identifyReceiver(const uint16_t productId) const {
    // Only these two PIDs are confirmed. Logitech has shipped many Unifying receiver
    // hardware revisions over the years with other PIDs; this list is intentionally
    // incomplete. An unrecognized PID just falls back to the receiver's raw
    // sysfs name, which is harmless.
    switch (productId) {
    case 0xC52B:
    case 0xC532:
        return "Unifying Receiver";
    default:
        return std::nullopt;
    }
}

std::optional<HidppReceiver::PairingInfo> UnifyingProtocolProbe::getPairingInfo(
    HidppReceiver& hidpp, const uint8_t deviceIndex) const {
    if (deviceIndex < 1 || deviceIndex > 6)
        return std::nullopt;

    const auto subAddress = static_cast<uint8_t>(0x20 + (deviceIndex - 1));
    const auto res = hidpp.readLongRegister(REG_PAIRING_INFORMATION, subAddress);
    if (!res.success || res.data.size() < 4) {
        std::println("[OptiDeck] UnifyingProtocolProbe({}): sub-address 0x{:02x} -- {}",
                     deviceIndex, subAddress, hidppDescribeFailure(res));
        return std::nullopt;
    }

    HidppReceiver::PairingInfo info;
    info.wirelessProductId = static_cast<uint16_t>(res.data[2] | (res.data[3] << 8));
    info.deviceKind = res.data[1] & 0x0F;
    std::println("[OptiDeck] UnifyingProtocolProbe({}): HIT at sub-address 0x{:02x} -> wpid=0x{:04x}, kind={}",
                 deviceIndex, subAddress, info.wirelessProductId, info.deviceKind);
    return info;
}

std::optional<std::string> UnifyingProtocolProbe::getDeviceName(
    HidppReceiver&, const uint8_t deviceIndex) const {
    // Not implemented; see the header comment for how to fill this in
    // once real Unifying hardware is available to verify against.
    std::println("[OptiDeck] UnifyingProtocolProbe({}): name lookup not yet implemented", deviceIndex);
    return std::nullopt;
}