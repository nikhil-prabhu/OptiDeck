#include "HidppReceiver.h"
#include "../HidppProtocol.h"
#include "HidppDiagnostics.h"

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <print>

using namespace HidppProtocol;

namespace {
void drainPending(const int fd) {
    uint8_t drain[64];
    while (::read(fd, drain, sizeof(drain)) > 0) {
    }
}
} // namespace

HidppReceiver::HidppReceiver(const std::string& devNode) {
    m_fd = ::open(devNode.c_str(), O_RDWR | O_NONBLOCK);
    if (m_fd < 0) {
        std::println(stderr, "[OptiDeck] HidppReceiver: failed to open {}: {} (errno={})",
                     devNode, std::strerror(errno), errno);
    }
}

HidppReceiver::~HidppReceiver() {
    if (m_fd >= 0)
        ::close(m_fd);
}

HidppReceiver::RegisterReadResult HidppReceiver::readShortRegister(
    const uint8_t address, const uint8_t p0, const uint8_t p1, const uint8_t p2, const int timeoutMs) const {
    RegisterReadResult result;
    if (m_fd < 0)
        return result;

    drainPending(m_fd);

    const uint8_t request[LEN_SHORT] = {
        REPORT_ID_SHORT, DEVICE_INDEX_RECEIVER, SUBID_GET_REGISTER, address, p0, p1, p2};

    if (::write(m_fd, request, sizeof(request)) != static_cast<ssize_t>(sizeof(request))) {
        std::println(stderr, "[OptiDeck] readShortRegister(0x{:02x}): write failed (errno={} {})",
                     address, errno, std::strerror(errno));
        result.writeFailed = true;
        return result;
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
    while (std::chrono::steady_clock::now() < deadline) {
        const auto remainingMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                                     deadline - std::chrono::steady_clock::now())
                                     .count();
        if (remainingMs <= 0)
            break;

        pollfd pfd{.fd = m_fd, .events = POLLIN, .revents = 0};
        if (::poll(&pfd, 1, static_cast<int>(remainingMs)) <= 0)
            break;

        uint8_t response[32] = {};
        if (const ssize_t n = ::read(m_fd, response, sizeof(response)); n < static_cast<ssize_t>(LEN_SHORT))
            continue;
        if (response[0] != REPORT_ID_SHORT)
            continue; // ignore long (0x11) traffic here

        if (response[2] == SUBID_ERROR && response[3] == SUBID_GET_REGISTER && response[4] == address) {
            result.success = false;
            result.errorCode = response[5];
            return result;
        }
        if (response[2] == SUBID_GET_REGISTER && response[3] == address) {
            result.success = true;
            result.data.assign(response + 4, response + 7); // 3 data bytes
            return result;
        }
        // else: unrelated traffic sharing the fd; keep waiting
    }

    result.timedOut = true;
    return result;
}

HidppReceiver::RegisterReadResult HidppReceiver::readLongRegister(
    const uint8_t address, const uint8_t p0, const uint8_t p1, const uint8_t p2, const int timeoutMs) const {
    RegisterReadResult result;
    if (m_fd < 0)
        return result;

    drainPending(m_fd);

    // Note: the *request* for GET_LONG_REGISTER is a SHORT report (7 bytes);
    // only the successful reply is long. This matches the spec exactly.
    const uint8_t request[LEN_SHORT] = {
        REPORT_ID_SHORT, DEVICE_INDEX_RECEIVER, SUBID_GET_LONG_REGISTER, address, p0, p1, p2};

    if (::write(m_fd, request, sizeof(request)) != static_cast<ssize_t>(sizeof(request))) {
        std::println(stderr, "[OptiDeck] readLongRegister(0x{:02x}, p0=0x{:02x}): write failed (errno={} {})",
                     address, p0, errno, std::strerror(errno));
        result.writeFailed = true;
        return result;
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
    while (std::chrono::steady_clock::now() < deadline) {
        const auto remainingMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                                     deadline - std::chrono::steady_clock::now())
                                     .count();
        if (remainingMs <= 0)
            break;

        pollfd pfd{.fd = m_fd, .events = POLLIN, .revents = 0};
        if (::poll(&pfd, 1, static_cast<int>(remainingMs)) <= 0)
            break;

        uint8_t response[32] = {};
        const ssize_t n = ::read(m_fd, response, sizeof(response));
        if (n <= 0)
            continue;

        // Error replies are always short, even for a long-register request.
        if (n >= static_cast<ssize_t>(LEN_SHORT) && response[0] == REPORT_ID_SHORT &&
            response[2] == SUBID_ERROR && response[3] == SUBID_GET_LONG_REGISTER && response[4] == address) {
            result.success = false;
            result.errorCode = response[5];
            return result;
        }
        if (n >= static_cast<ssize_t>(LEN_LONG) && response[0] == REPORT_ID_LONG &&
            response[2] == SUBID_GET_LONG_REGISTER && response[3] == address) {
            result.success = true;
            result.data.assign(response + 4, response + 20); // 16 data bytes
            return result;
        }
        // else: unrelated traffic sharing the fd; keep waiting
    }

    result.timedOut = true;
    return result;
}

std::optional<uint8_t> HidppReceiver::getConnectedDeviceCount() const {
    const auto res = readShortRegister(REG_CONNECTION_STATE);
    if (!res.success) {
        std::println("[OptiDeck] getConnectedDeviceCount: {}", hidppDescribeFailure(res));
        return std::nullopt;
    }
    // Response layout per spec: "10 ix 81 02 00 r1 r2"; data = [echoed p0, r1=count, r2]
    if (res.data.size() < 2)
        return std::nullopt;
    return res.data[1];
}

std::vector<HidppReceiver::PairingSweepHit> HidppReceiver::sweepPairingRegister(const int perAttemptTimeoutMs) const {
    std::vector<PairingSweepHit> hits;
    std::println("[OptiDeck] sweepPairingRegister: starting full 0x00-0xFF sweep of register 0x{:02x} (diagnostic only)",
                 REG_PAIRING_INFORMATION);

    for (int nn = 0; nn <= 0xFF; ++nn) {
        if (auto res = readLongRegister(REG_PAIRING_INFORMATION, static_cast<uint8_t>(nn), 0, 0, perAttemptTimeoutMs);
            res.success) {
            std::println("[OptiDeck] sweepPairingRegister: HIT at sub-address 0x{:02x} -> {}",
                         nn, hidppHexDump(res.data.data(), res.data.size()));
            hits.push_back({.subAddress = static_cast<uint8_t>(nn), .data = res.data});
        }
    }

    std::println("[OptiDeck] sweepPairingRegister: done, {} hit(s)", hits.size());
    return hits;
}

void HidppReceiver::probeDeviceName(const uint8_t deviceIndex) const {
    if (deviceIndex < 1 || deviceIndex > 6)
        return;

    // Cheap first guess: same per-device row used for pairing info, but
    // with a few candidate "page" parameters.
    const auto candidateSubAddress = static_cast<uint8_t>(0x51 + (deviceIndex - 1) * 0x10);
    std::println("[OptiDeck] probeDeviceName({}): trying sub-address 0x{:02x} with a few candidate pages",
                 deviceIndex, candidateSubAddress);
    for (uint8_t page = 0; page <= 4; ++page) {
        if (const auto res = readLongRegister(REG_PAIRING_INFORMATION, candidateSubAddress, page);
            res.success) {
            std::println("[OptiDeck] probeDeviceName({}): sub-address 0x{:02x} page {} -> {}",
                         deviceIndex, candidateSubAddress, page, hidppHexDump(res.data.data(), res.data.size()));
        }
    }

    // Fallback: the name may live at a different base address entirely.
    // Sweep the full sub-address range with page=1 fixed, since a page=0-only
    // sweep would miss anything that only replies when this parameter is set.
    std::println("[OptiDeck] probeDeviceName({}): sweeping full sub-address range with page=1 (diagnostic only)",
                 deviceIndex);
    for (int nn = 0; nn <= 0xFF; ++nn) {
        if (const auto res = readLongRegister(REG_PAIRING_INFORMATION, static_cast<uint8_t>(nn), 0x01);
            res.success) {
            std::println("[OptiDeck] probeDeviceName({}): HIT at sub-address 0x{:02x} (page 1) -> {}",
                         deviceIndex, nn, hidppHexDump(res.data.data(), res.data.size()));
        }
    }
    std::println("[OptiDeck] probeDeviceName({}): done", deviceIndex);
}