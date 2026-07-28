#ifndef OPTIDECK_V4L2SCANNER_H
#define OPTIDECK_V4L2SCANNER_H

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>

namespace fs = std::filesystem;

struct V4L2MenuItem {
    uint32_t index{0};
    std::string name;
};

struct V4L2Control {
    uint32_t id{0};
    std::string name;
    uint32_t type{0}; // V4L2 control type (e.g., V4L2_CTRL_TYPE_MENU)
    int32_t minimum{0};
    int32_t maximum{0};
    int32_t step{0};
    int32_t defaultValue{0};
    int32_t currentValue{0};
    bool isInactive{false};
    std::vector<V4L2MenuItem> menuItems;
};

struct V4L2Camera {
    std::string devicePath;
    std::string cardName;
    std::string driverName;
    std::vector<V4L2Control> controls;
};

class V4L2Scanner {
public:
    // Scans `/dev/video*` for valid video capture devices and their supported controls
    static std::vector<V4L2Camera> scanCameras();
};

#endif //OPTIDECK_V4L2SCANNER_H
