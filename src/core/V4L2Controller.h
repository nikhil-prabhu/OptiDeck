#ifndef OPTIDECK_V4L2CONTROLLER_H
#define OPTIDECK_V4L2CONTROLLER_H

#include <string>
#include <cstdint>

class V4L2Controller {
public:
    // Sets a specific V4L2 control value (e.g. Brightness, Contrast, Exposure)
    static bool setControl(const std::string& devicePath, uint32_t controlId, int32_t value);

    // Reads the current value of a specific V4L2 control
    static int32_t getControl(const std::string& devicePath, uint32_t controlId);
};

#endif //OPTIDECK_V4L2CONTROLLER_H