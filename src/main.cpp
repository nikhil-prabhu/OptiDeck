#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <print>
#include <linux/videodev2.h>

#include "core/V4L2Scanner.h"
#include "core/V4L2Controller.h"

#define APP_VERSION "0.1.0"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationVersion(APP_VERSION);

    std::println("[OptiDeck] Scanning for cameras via V4L2...");

    for (auto cameras = V4L2Scanner::scanCameras(); const auto &[devicePath, cardName, driverName, controls] : cameras) {
        std::println("  Found Camera: {} ({}) at {}", cardName, driverName, devicePath);

        // Test modifying Brightness on the primary capture node (e.g. /dev/video0)
        if (int32_t currentBrightness = V4L2Controller::getControl(devicePath, V4L2_CID_BRIGHTNESS); currentBrightness != -1) {
            std::println("  [Test] Original Brightness: {}", currentBrightness);

            // Set new brightness
            if (int32_t testValue = (currentBrightness == 128) ? 140 : 128; V4L2Controller::setControl(devicePath, V4L2_CID_BRIGHTNESS, testValue)) {
                std::println("  [Test] Successfully updated Brightness to {}", testValue);
            }
        }
    }

    QQmlApplicationEngine engine;
    engine.loadFromModule("OptiDeck", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QGuiApplication::exec();
}