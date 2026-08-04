#ifndef OPTIDECK_BOLTPROTOCOLPROBE_H
#define OPTIDECK_BOLTPROTOCOLPROBE_H

#include "ReceiverProtocolProbe.h"

// Bolt receivers. Every formula here is verified against real hardware
// (a 046d:c548 receiver + paired MX Master 3S): pairing info lives at
// register 0xB5, sub-address 0x51 + (deviceIndex-1)*0x10; device name at the
// same register, sub-address 0x61 + (deviceIndex-1)*0x10, read with page
// parameter 1.
class BoltProtocolProbe final : public ReceiverProtocolProbe {
public:
    [[nodiscard]] std::string_view name() const override { return "Bolt"; }

    [[nodiscard]] std::optional<std::string> identifyReceiver(uint16_t productId) const override;

    [[nodiscard]] std::optional<HidppReceiver::PairingInfo> getPairingInfo(
        HidppReceiver& hidpp, uint8_t deviceIndex) const override;

    [[nodiscard]] std::optional<std::string> getDeviceName(
        HidppReceiver& hidpp, uint8_t deviceIndex) const override;
};

#endif // OPTIDECK_BOLTPROTOCOLPROBE_H