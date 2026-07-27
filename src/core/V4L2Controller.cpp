#include "V4L2Controller.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <print>

bool V4L2Controller::setControl(const std::string& devicePath, uint32_t controlId, const int32_t value) {
    const int fd = ::open(devicePath.c_str(), O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        std::println(stderr, "[OptiDeck] Failed to open {} for writing control {:#010x}", devicePath, controlId);
        return false;
    }

    v4l2_control ctrl{
        .id = controlId,
        .value = value
    };

    const bool success = (::ioctl(fd, VIDIOC_S_CTRL, &ctrl) == 0);
    if (!success) {
        std::println(stderr, "[OptiDeck] VIDIOC_S_CTRL failed on {} for control {:#010x}", devicePath, controlId);
    }

    ::close(fd);
    return success;
}

int32_t V4L2Controller::getControl(const std::string& devicePath, uint32_t controlId) {
    const int fd = ::open(devicePath.c_str(), O_RDWR | O_NONBLOCK);
    if (fd < 0) return -1;

    v4l2_control ctrl{.id = controlId, .value = 0};
    int32_t result = -1;

    if (::ioctl(fd, VIDIOC_G_CTRL, &ctrl) == 0) {
        result = ctrl.value;
    }

    ::close(fd);
    return result;
}