#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "graphcore.h"
#include "graphnode.h"

void fillGraph(GraphCore &graphCore)
{
    const QString nodeNameTemplate = QStringLiteral("Node_%1");
    const QStringList portNameTemplates = { QStringLiteral("Integer_%1"), QStringLiteral("Double_%1"), QStringLiteral("Expr_%1"), QStringLiteral("Bool_%1") };
    for (int n = 1, x = 50, y = 50, p = 1; n <= 3; ++n, x += 330, y += 80) {
        const QString &nodeName = nodeNameTemplate.arg(n);
        graphCore.addGraphNode(nodeName, x, y);
        GraphNode *node = graphCore.findNode(nodeName);
        node->addInputPort(portNameTemplates.at(0).arg(p++), x + y);
        node->addInputPort(portNameTemplates.at(1).arg(p++), x * y * 0.3);
        node->addInputPort(portNameTemplates.at(2).arg(p++), QString(QStringLiteral("%1 + %2 * %3")).arg(x).arg(y).arg(n));
        bool flag = bool(p % 2);
        node->addInputPort(portNameTemplates.at(3).arg(p++), flag);

        node->addOutputPort(portNameTemplates.at(0).arg(p++), x + y);
        node->addOutputPort(portNameTemplates.at(1).arg(p++), x * y * 0.45);
        node->addOutputPort(portNameTemplates.at(2).arg(p++), QString(QStringLiteral("%1 - %2 / %3")).arg(x).arg(y).arg(n));
        node->addOutputPort(portNameTemplates.at(3).arg(p++), !flag);
    }
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<QObjectList>("QObjectList");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QCoreApplication::setApplicationName(QStringLiteral("Graph View"));
    QCoreApplication::setOrganizationName(QStringLiteral("For InstaLOD"));
    QCoreApplication::setApplicationVersion(QLatin1String(QT_VERSION_STR));

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    GraphCore graphCore;
    fillGraph(graphCore);
    context->setContextProperty(QStringLiteral("graphCore"), &graphCore);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
