#ifndef OPTIDECK_UNIFYINGPROTOCOLPROBE_H
#define OPTIDECK_UNIFYINGPROTOCOLPROBE_H

#include "ReceiverProtocolProbe.h"

// Classic Unifying receivers. The pairing-info sub-address formula
// (0x20 + deviceIndex-1) comes straight from Logitech's own leaked HID++ 1.0
// specification, so it's on solid documentary footing; but unlike Bolt's,
// it has NOT been verified against real Unifying hardware, since I currently
// don't own any.
//
// Device name lookup isn't implemented at all yet. If you have a Unifying
// receiver and want to fill it in:
//   1. Confirm getPairingInfo() actually gets a HIT on real hardware first.
//   2. Call HidppReceiver::probeDeviceName(deviceIndex) or
//      sweepPairingRegister() to find the name sub-address/page empirically.
//   3. Hardcode the verified formula in getDeviceName() below.
class UnifyingProtocolProbe final : public ReceiverProtocolProbe {
public:
    [[nodiscard]] std::string_view name() const override { return "Unifying"; }

    [[nodiscard]] std::optional<std::string> identifyReceiver(uint16_t productId) const override;

    [[nodiscard]] std::optional<HidppReceiver::PairingInfo> getPairingInfo(
        HidppReceiver& hidpp, uint8_t deviceIndex) const override;

    [[nodiscard]] std::optional<std::string> getDeviceName(
        HidppReceiver& hidpp, uint8_t deviceIndex) const override;
};

#endif // OPTIDECK_UNIFYINGPROTOCOLPROBE_H