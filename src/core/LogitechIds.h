#ifndef OPTIDECK_LOGITECHIDS_H
#define OPTIDECK_LOGITECHIDS_H

#include <cstdint>

// Logitech related vendor and product IDs.
namespace LogitechIds {
constexpr uint16_t VENDOR_ID = 0x046D;

namespace Receiver {
constexpr uint16_t BOLT = 0xC548;

// Unifying receivers.
// Logitech has shipped many Unifying receiver hardware revisions over the years with
// other PIDs; this list is intentionally incomplete. An unrecognized PID just falls
// back to the receiver's raw sysfs name, which is harmless.
constexpr uint16_t UNIFYING_C52B = 0xC52B;
constexpr uint16_t UNIFYING_C532 = 0xC532;
} // namespace Receiver

// Wireless product IDs, as reported by a receiver's pairing-info register.
// These are NOT necessarily the same as a device's direct USB/Bluetooth PID.
// e.g. M196 here is its Bluetooth-direct PID (since that's how it's connected),
// while the MX Master 3S entries are wireless PIDs read via a Bolt receiver.
namespace Mouse {
constexpr uint16_t MX_MASTER_3S_BOLT = 0xB034;
constexpr uint16_t MX_MASTER_3S_MAC = 0xB035;
constexpr uint16_t MX_MASTER_3 = 0xB023;

constexpr uint16_t M196 = 0xB03F;
} // namespace Mouse

namespace Keyboard {
constexpr uint16_t MX_KEYS = 0xB353;
}

} // namespace LogitechIds

#endif // OPTIDECK_LOGITECHIDS_H
