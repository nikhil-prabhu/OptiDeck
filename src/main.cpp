#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "core/DeviceManager.h"
#include "core/ThemeImageProvider.h"

#define APP_VERSION "0.1.0"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[]) {
    const QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationVersion(APP_VERSION);

    DeviceManager deviceManager;

    QQmlApplicationEngine engine;
    engine.addImageProvider(u"theme"_s, new ThemeImageProvider());
    engine.rootContext()->setContextProperty("deviceManager", &deviceManager);

    const QUrl url(u"qrc:/qt/qml/OptiDeck/src/ui/Main.qml"_s);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](const QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return QGuiApplication::exec();
}
