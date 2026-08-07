#ifndef OPTIDECK_HIDPPPROTOCOL_H
#define OPTIDECK_HIDPPPROTOCOL_H

#include <cstddef>
#include <cstdint>

// HID++ 1.0 register-access wire-format constants, per Logitech's own
// (leaked, publicly available) "hidpp 1.0 excerpt for public release"
// specification. Shared by `HidppReceiver` (the transport implementation)
// and every `ReceiverProtocolProbe` (which builds on these to implement
// a specific receiver family's register sub-address layout).
namespace HidppProtocol {
constexpr uint8_t REPORT_ID_SHORT = 0x10;
constexpr uint8_t REPORT_ID_LONG = 0x11;
constexpr std::size_t LEN_SHORT = 7;
constexpr std::size_t LEN_LONG = 20;

constexpr uint8_t DEVICE_INDEX_RECEIVER = 0xFF;

constexpr uint8_t SUBID_GET_REGISTER = 0x81;
constexpr uint8_t SUBID_GET_LONG_REGISTER = 0x83;
constexpr uint8_t SUBID_ERROR = 0x8F;

// Registers
constexpr uint8_t REG_CONNECTION_STATE = 0x02;    // read: number of connected devices (receiver-generic)
constexpr uint8_t REG_PAIRING_INFORMATION = 0xB5; // per-device pairing details; sub-address layout varies by receiver family
} // namespace HidppProtocol

#endif // OPTIDECK_HIDPPPROTOCOL_H
