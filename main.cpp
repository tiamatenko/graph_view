#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "graphcore.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QCoreApplication::setApplicationName(QStringLiteral("Graph View"));
    QCoreApplication::setOrganizationName(QStringLiteral("For InstaLOD"));
    QCoreApplication::setApplicationVersion(QLatin1String(QT_VERSION_STR));

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    GraphCore graphCore;
    context->setContextProperty(QStringLiteral("contextGraphCore"), &graphCore);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
