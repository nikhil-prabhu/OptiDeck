#ifndef OPTIDECK_HIDPPDIAGNOSTICS_H
#define OPTIDECK_HIDPPDIAGNOSTICS_H

#include <cstdint>
#include <cstdio>
#include <string>

#include "HidppReceiver.h"

// Small, shared logging helpers used by HidppReceiver's own diagnostic
// methods and by every ReceiverProtocolProbe implementation, so each probe
// doesn't need to reinvent hex-dumping or failure-reason formatting.

inline std::string hidppHexDump(const uint8_t* data, const size_t len) {
    std::string out;
    char buf[4];
    for (size_t i = 0; i < len; ++i) {
        std::snprintf(buf, sizeof(buf), "%02x ", data[i]);
        out += buf;
    }
    return out;
}

inline std::string hidppDescribeFailure(const HidppReceiver::RegisterReadResult& res) {
    if (res.writeFailed)
        return "write failed";
    if (res.timedOut)
        return "no reply within timeout";
    char buf[32];
    std::snprintf(buf, sizeof(buf), "device reported error 0x%02x", res.errorCode);
    return buf;
}

#endif // OPTIDECK_HIDPPDIAGNOSTICS_H