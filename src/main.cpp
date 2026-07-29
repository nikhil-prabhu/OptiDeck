#include <KIconTheme>
#include <KLocalizedString>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "core/DeviceManager.h"

int main(int argc, char* argv[]) {
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("optideck");
    QApplication::setOrganizationName(QStringLiteral("nikhil-prabhu"));
    QApplication::setOrganizationDomain(QStringLiteral("com.github.nikhil-prabhu"));
    QApplication::setApplicationName(QStringLiteral("OptiDeck"));
    QApplication::setDesktopFileName(QStringLiteral("com.nikhil-prabhu.optideck"));

    DeviceManager deviceManager;
    QQmlApplicationEngine engine;

    // TODO: deprecated; replace with KLocalizedQmlContext from KF6::I18nQml when feasible
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.rootContext()->setContextProperty("deviceManager", &deviceManager);
    engine.loadFromModule("com.nikhil-prabhu.optideck", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QApplication::exec();
}