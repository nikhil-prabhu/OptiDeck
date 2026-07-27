#include <QGuiApplication>
#include <QQmlApplicationEngine>

#define APP_VERSION "0.1.0"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QGuiApplication::setApplicationVersion(APP_VERSION);

    engine.loadFromModule("OptiDeck", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QGuiApplication::exec();
}