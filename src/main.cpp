#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <print>

#include "core/V4L2Scanner.h"

#define APP_VERSION "0.1.0"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationVersion(APP_VERSION);

    std::println("[OptiDeck] Scanning for cameras via V4L2...");

    for (const auto cameras = V4L2Scanner::scanCameras(); const auto& [devicePath, cardName, driverName, controls]:
         cameras) {
        std::println("  Found Camera: {} ({}) at {}", cardName, driverName, devicePath);

        for (const auto& ctrl: controls) {
            std::println("    - Control: {} | Range: [{}-{}] | Current: {}",
                         ctrl.name, ctrl.minimum, ctrl.maximum, ctrl.currentValue);
        }
    }

    QQmlApplicationEngine engine;
    engine.loadFromModule("OptiDeck", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QGuiApplication::exec();
}
