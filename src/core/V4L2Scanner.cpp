#include "V4L2Scanner.h"

#include <print>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

std::vector<V4L2Camera> V4L2Scanner::scanCameras() {
    std::vector<V4L2Camera> cameras;
    const fs::path devPath{"/dev"};

    if (!fs::exists(devPath)) return cameras;

    for (const auto &entry: fs::directory_iterator(devPath)) {
        if (std::string filename = entry.path().filename().string(); !filename.starts_with("video")) continue;

        std::string path = entry.path().string();
        const int fd = ::open(path.c_str(), O_RDWR | O_NONBLOCK);
        if (fd < 0) continue;

        v4l2_capability cap{};
        if (::ioctl(fd, VIDIOC_QUERYCAP, &cap) == 0) {
            // Ensure device supports video capture (and is not a metadata stream)
            const uint32_t capabilities = (cap.capabilities & V4L2_CAP_DEVICE_CAPS)
                                              ? cap.device_caps
                                              : cap.capabilities;

            if ((capabilities & V4L2_CAP_VIDEO_CAPTURE) && !(capabilities & V4L2_CAP_META_CAPTURE)) {
                V4L2Camera cam{
                    .devicePath = path,
                    .cardName = reinterpret_cast<const char *>(cap.card),
                    .driverName = reinterpret_cast<const char *>(cap.driver)
                };

                // Query basic control ranges (Brightness, Contrast, Exposure, etc.)
                v4l2_queryctrl queryctrl{};
                queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;

                while (::ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == 0) {
                    if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
                        v4l2_control ctrl{.id = queryctrl.id};
                        int currentVal = queryctrl.default_value;
                        if (::ioctl(fd, VIDIOC_G_CTRL, &ctrl) == 0) {
                            currentVal = ctrl.value;
                        }

                        cam.controls.push_back(V4L2Control{
                            .id = queryctrl.id,
                            .name = reinterpret_cast<const char *>(queryctrl.name),
                            .minimum = queryctrl.minimum,
                            .maximum = queryctrl.maximum,
                            .step = queryctrl.step,
                            .defaultValue = queryctrl.default_value,
                            .currentValue = currentVal
                        });
                    }
                    queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
                }

                cameras.push_back(cam);
            }
        }

        ::close(fd);
    }

    return cameras;
}
