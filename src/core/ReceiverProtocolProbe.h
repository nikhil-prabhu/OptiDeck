#ifndef OPTIDECK_RECEIVERPROTOCOLPROBE_H
#define OPTIDECK_RECEIVERPROTOCOLPROBE_H

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "HidppReceiver.h"

// One receiver family's knowledge of register 0xB5's layout and how to
// recognize its own receivers. Add support for a new family (Nano,
// Lightspeed, ...) by implementing this interface and registering an
// instance in ReceiverProbeRegistry; no other code needs to change.
class ReceiverProtocolProbe {
public:
    virtual ~ReceiverProtocolProbe() = default;

    // Short, human-readable name for logging (e.g. "Bolt", "Unifying").
    [[nodiscard]] virtual std::string_view name() const = 0;

    // If productId is a receiver this family recognizes, returns a friendly
    // display name (e.g. "Bolt Receiver"). Returns nullopt otherwise. Exists
    // because a receiver's raw USB descriptor name is often generic (this
    // project's own Bolt receiver reports itself simply as "Logitech USB
    // Receiver") and doesn't reliably identify which family it belongs to.
    [[nodiscard]] virtual std::optional<std::string> identifyReceiver(uint16_t productId) const = 0;

    // Attempts to read pairing info for deviceIndex (1..6) using this
    // family's register layout. Returns nullopt if this probe's layout
    // doesn't apply here (wrong family, empty slot, or a communication
    // failure); the caller should just try the next probe.
    [[nodiscard]] virtual std::optional<HidppReceiver::PairingInfo> getPairingInfo(
        HidppReceiver& hidpp, uint8_t deviceIndex) const = 0;

    // Attempts to read the device's display name (e.g. "MX Master 3S").
    // Returns nullopt if this family's name lookup isn't implemented yet, or
    // the read fails; callers should fall back to a generic placeholder.
    [[nodiscard]] virtual std::optional<std::string> getDeviceName(
        HidppReceiver& hidpp, uint8_t deviceIndex) const = 0;
};

#endif // OPTIDECK_RECEIVERPROTOCOLPROBE_H