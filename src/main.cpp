#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <print>
#include <qqmlcontext.h>
#include <linux/videodev2.h>

#include "core/CameraManager.h"
#include "core/V4L2Scanner.h"
#include "core/V4L2Controller.h"

#define APP_VERSION "0.1.0"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationVersion(APP_VERSION);

    CameraManager cameraManager;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cameraManager", &cameraManager);
    engine.loadFromModule("OptiDeck", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QGuiApplication::exec();
}