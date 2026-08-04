#ifndef OPTIDECK_HIDPPRECEIVER_H
#define OPTIDECK_HIDPPRECEIVER_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// Talks to a Logitech receiver using the HID++ 1.0 register-access protocol,
// per Logitech's own (leaked, publicly available) "hidpp 1.0 excerpt for
// public release" specification:
//
//   GET_REGISTER (short):      10 ix 81 <addr> <p0> <p1> <p2>
//                          ->  10 ix 81 <addr> <p0> <r1> <r2>          (success)
//                          ->  10 ix 8F 81 <addr> <errorCode> 00       (error)
//
//   GET_LONG_REGISTER:         10 ix 83 <addr> <p0> <p1> <p2>
//                          ->  11 ix 83 <addr> <16 data bytes>         (success)
//                          ->  10 ix 8F 83 <addr> <errorCode> 00       (error)
//
// Register 0x02 ("Connection State") returns the number of connected
// devices on read; this is receiver-generic, not specific to any one
// receiver family.
//
// Register 0xB5 ("Pairing information") holds per-device pairing details,
// at a sub-address that varies by receiver family (e.g. Bolt vs. classic
// Unifying use different layouts). This class knows nothing about those
// specific layouts; that knowledge lives in ReceiverProtocolProbe
// implementations (see ReceiverProtocolProbe.h), which use the generic
// readShortRegister()/readLongRegister() primitives below. sweepPairingRegister()
// and probeDeviceName() exist here as reusable discovery tools for finding a
// new family's layout empirically.
class HidppReceiver {
public:
    struct RegisterReadResult {
        bool success{false};
        bool writeFailed{false};   // true if the write() itself failed (e.g. EPIPE on this interface)
        bool timedOut{false};      // true if we got no reply at all within the deadline
        uint8_t errorCode{0};      // valid when !success, !writeFailed, and !timedOut (a genuine device error reply)
        std::vector<uint8_t> data; // register value: 3 bytes (short) or 16 bytes (long), valid when success
    };

    struct PairingSweepHit {
        uint8_t subAddress{0};
        std::vector<uint8_t> data;
    };

    struct PairingInfo {
        uint16_t wirelessProductId{0};
        uint8_t deviceKind{0}; // low nibble of the kind byte, e.g. 2 = mouse
    };

    explicit HidppReceiver(const std::string& devNode);
    ~HidppReceiver();

    HidppReceiver(const HidppReceiver&) = delete;
    HidppReceiver& operator=(const HidppReceiver&) = delete;

    [[nodiscard]] bool isOpen() const { return m_fd >= 0; }

    [[nodiscard]] RegisterReadResult readShortRegister(uint8_t address, uint8_t p0 = 0, uint8_t p1 = 0, uint8_t p2 = 0, int timeoutMs = 300) const;
    [[nodiscard]] RegisterReadResult readLongRegister(uint8_t address, uint8_t p0 = 0, uint8_t p1 = 0, uint8_t p2 = 0, int timeoutMs = 300) const;

    // Register 0x02: number of currently connected devices, per spec.
    [[nodiscard]] std::optional<uint8_t> getConnectedDeviceCount() const;

    // DIAGNOSTIC ONLY; slow (up to ~256 round trips). Sweeps register
    // 0xB5's sub-address space (0x00-0xFF) and returns every sub-address
    // that gets a non-error reply, with the raw data. Useful for finding a
    // new receiver family's pairing-info layout empirically.
    [[nodiscard]] std::vector<PairingSweepHit> sweepPairingRegister(int perAttemptTimeoutMs = 60) const;

    // DIAGNOSTIC ONLY; looking for a device-name string. Checks a given
    // per-device row (a handful of candidate pages, cheap) first, then falls
    // back to a full sub-address sweep with page=1 fixed. Useful for finding
    // a new receiver family's name lookup empirically.
    void probeDeviceName(uint8_t deviceIndex) const;

private:
    int m_fd{-1};
};

#endif // OPTIDECK_HIDPPRECEIVER_H