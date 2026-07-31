#ifndef OPTIDECK_RECEIVERPROBEREGISTRY_H
#define OPTIDECK_RECEIVERPROBEREGISTRY_H

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "HidppReceiver.h"
#include "ReceiverProtocolProbe.h"

// Tries each known receiver-family probe in turn for a given device
// index, or to identify a receiver's product ID, and returns the first
// genuine hit.
//
// This is intentionally sequential, not parallel: hidraw's read queue is
// per-file-descriptor, but the underlying USB device is a single physical
// channel, and there's no evidence this (or any) receiver's firmware
// tolerates concurrent HID++ requests cleanly. Given each probe only runs
// once per receiver per scan and takes at most a few hundred ms, the
// correctness risk isn't worth the negligible speed gain. If a future
// contributor verifies concurrent access is safe on their hardware, probe()
// below is a single, isolated place to swap the loop for e.g. std::async.
//
// Add a new receiver family by writing a ReceiverProtocolProbe subclass and
// registering an instance in the constructor; no other code needs to
// change.
class ReceiverProbeRegistry {
public:
    struct ProbedDevice {
        HidppReceiver::PairingInfo pairingInfo;
        std::string deviceName;          // generic placeholder if this family has no name lookup yet
        std::string_view protocolFamily; // which probe found it, e.g. "Bolt"; for logging
    };

    ReceiverProbeRegistry();

    // Tries each registered probe for deviceIndex (1..6), in order, until
    // one reports a genuine pairing hit. Returns nullopt if none do.
    [[nodiscard]] std::optional<ProbedDevice> probeDevice(HidppReceiver& hidpp, uint8_t deviceIndex) const;

    // Tries each registered probe's identifyReceiver() for productId.
    // Returns a friendly name (e.g. "Bolt Receiver") if any probe recognizes
    // it, or nullopt if none do; callers should fall back to the
    // receiver's raw sysfs name in that case.
    [[nodiscard]] std::optional<std::string> identifyReceiver(uint16_t productId) const;

private:
    std::vector<std::unique_ptr<ReceiverProtocolProbe>> m_probes;
};

#endif // OPTIDECK_RECEIVERPROBEREGISTRY_H