#include "ReceiverProbeRegistry.h"

#include "BoltProtocolProbe.h"
#include "UnifyingProtocolProbe.h"

ReceiverProbeRegistry::ReceiverProbeRegistry() {
    m_probes.push_back(std::make_unique<UnifyingProtocolProbe>());
    m_probes.push_back(std::make_unique<BoltProtocolProbe>());
    // Add new receiver families here (e.g. Nano, Lightspeed).
}

std::optional<ReceiverProbeRegistry::ProbedDevice> ReceiverProbeRegistry::probeDevice(
    HidppReceiver& hidpp, const uint8_t deviceIndex) const {
    for (const auto& probe : m_probes) {
        auto info = probe->getPairingInfo(hidpp, deviceIndex);
        if (!info)
            continue;

        ProbedDevice result;
        result.pairingInfo = *info;
        result.protocolFamily = probe->name();
        result.deviceName = probe->getDeviceName(hidpp, deviceIndex).value_or("Logitech Wireless Device");
        return result;
    }
    return std::nullopt;
}

std::optional<std::string> ReceiverProbeRegistry::identifyReceiver(const uint16_t productId) const {
    for (const auto& probe : m_probes) {
        if (auto friendlyName = probe->identifyReceiver(productId)) {
            return friendlyName;
        }
    }
    return std::nullopt;
}