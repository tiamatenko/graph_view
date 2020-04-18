#include "graphconnection.h"
#include "graphcore.h"
#include "graphcore.h"
#include "graphnode.h"
#include "graphnodeport.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QVector>
#include <QDebug>

/**
 * @brief The GraphCore class owns and managers all objects of the graph view.
 * Also it can create and remove objects and conntions. save to and load from file
*/

/**
 * @brief GraphCore::GraphCore ctor
 * @param parent
 */
GraphCore::GraphCore(QObject *parent)
    : QObject(parent)
    , m_sourceFileName(tr("<Empty>"))
{
}

/**
 * @brief GraphCore::save saves all objects to the source file
 */
void GraphCore::save()
{
    saveTo(sourceFileName());
}

/**
 * @brief GraphCore::saveAs saves all objects to a new file
 * @param fileName new file name
 */
void GraphCore::saveAs(const QString &fileName)
{
    if (!saveTo(sourceFileName()))
        return;

    if (sourceFileName() != fileName) {
        m_sourceFileName = fileName;
        emit sourceFileNameChanged(m_sourceFileName);
    }
}

/**
 * @brief GraphCore::load loads data from file
 * @param fileName a file name
 */
void GraphCore::load(const QString &fileName)
{
    if (!loadFrom(fileName))
        return;

    if (sourceFileName() != fileName) {
        m_sourceFileName = fileName;
        emit sourceFileNameChanged(m_sourceFileName);
    }
    emit graphChanged();
}

/**
 * @brief GraphCore::addGraphNode creates a new node
 * @param name of a new node
 */
GraphNode *GraphCore::addGraphNode(const QString &name, const QPointF &coord)
{
    if (name.isEmpty()) {
        emit errorOccurred(tr("Graph node name is empty"));
        return nullptr;
    }
    if (m_graphNodes.contains(name)) {
        emit errorOccurred(tr("Graph node '%1' already exists").arg(name));
        return nullptr;
    }
    GraphNode *node = new GraphNode(coord, name, this);
    m_graphNodes[name] = node;

    connect(node, &GraphNode::outputPortsChanged, this, &GraphCore::graphChanged);
    connect(node, &GraphNode::inputPortsChanged, this, &GraphCore::graphChanged);
    connect(node, &GraphNode::errorOccurred, this, &GraphCore::errorOccurred);
    emit graphChanged();
    return node;
}

/**
 * @brief GraphCore::removeGraphNode removes node
 * @param name of removed node
 */
void GraphCore::removeGraphNode(const QString &name)
{
    auto it = m_graphNodes.find(name);
    if (it == m_graphNodes.end()) {
        emit errorOccurred(tr("Graph node '%1' does not exist").arg(name));
        return;
    }
    QObject *node = it.value();
    m_graphNodes.erase(it);
    emit graphChanged();
    node->deleteLater();
}

/**
 * @brief GraphCore::addGraphConnection creates a new connection
 * @param src name of the source node
 * @param out output port name of the source node
 * @param dest name of the destination node
 * @param in input port name of the destination node
 */
GraphConnection *GraphCore::addGraphConnection(const QString &src, const QString &out, const QString &dest, const QString &in)
{
    const QString name = QString(QLatin1String("%1.%2->%3.%4")).arg(src, out, dest, in);
    if (m_graphConnections.contains(name)) {
        emit errorOccurred(tr("Connection '%1' already exists").arg(name));
        return nullptr;
    }
    GraphNode *sourceNode = qobject_cast<GraphNode *>(m_graphNodes.value(src));
    if (!sourceNode) {
        emit errorOccurred(tr("Unable to find '%1' node").arg(src));
        return nullptr;
    }
    GraphNode *destNode = qobject_cast<GraphNode *>(m_graphNodes.value(dest));
    if (!destNode) {
        emit errorOccurred(tr("Unable to find '%1' node").arg(dest));
        return nullptr;
    }
    GraphNodePort *outPort = sourceNode->outputPort(out);
    if (!outPort) {
        emit errorOccurred(tr("Unable to find '%1' port").arg(out));
        return nullptr;
    }
    GraphNodePort *inPort = destNode->inputPort(in);
    if (!inPort) {
        emit errorOccurred(tr("Unable to find '%1' port").arg(in));
        return nullptr;
    }

    GraphConnection *conn = new GraphConnection(outPort, inPort, name, this);
    m_graphConnections[name] = conn;
    connect(conn, &GraphConnection::errorOccurred, this, &GraphCore::errorOccurred);
    emit graphChanged();
    return conn;
}

/**
 * @brief GraphCore::removeGraphConnection removes connection
 * @param name of connection
 */
void GraphCore::removeGraphConnection(const QString &name)
{
    auto it = m_graphConnections.find(name);
    if (it == m_graphConnections.end()) {
        emit errorOccurred(tr("Connection '%1' does not exist").arg(name));
        return;
    }
    QObject *obj = it.value();
    m_graphConnections.erase(it);
    emit graphChanged();
    obj->deleteLater();
}

/**
 * @brief GraphCore::setZoomFactor sets a new zoom factor
 * @param zoomFactor value of a new zoom factor
 */
void GraphCore::setZoomFactor(double zoomFactor)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_zoomFactor, zoomFactor))
        return;

    m_zoomFactor = zoomFactor;
}

/**
 * @brief GraphCore::saveTo saves all current data to file
 * @param fileName file name
 * @return result of saving
 */
bool GraphCore::saveTo(const QString &fileName)
{
    QFile outputFile(fileName);
    if (!outputFile.open(QFile::WriteOnly | QFile::Truncate)) {
        emit errorOccurred(tr("Unable to open file '%1'").arg(fileName));
        return false;
    }

    QJsonObject sceneObject;
    sceneObject[getKey(JsonKeyID::ZoomFactor)] = m_zoomFactor;

    QJsonArray nodes;
    for (const auto n : m_graphNodes) {
        QJsonObject nodeObject;
        GraphNode *node = static_cast<GraphNode *>(n);
        nodeObject[getKey(JsonKeyID::Name)] = node->name();
        nodeObject[getKey(JsonKeyID::XCoord)] = node->coord().x();
        nodeObject[getKey(JsonKeyID::YCoord)] = node->coord().y();

        QJsonArray ports;
        for (const auto out : (node->outputPorts() + node->inputPorts())) {
            GraphNodePort *outPort = static_cast<GraphNodePort *>(out);
            QJsonObject portObject;
            portObject[getKey(JsonKeyID::Name)] = outPort->name();
            portObject[getKey(JsonKeyID::Type)] = outPort->portType();
            const QVariant &value = outPort->value();
            if (value.type() == QVariant::Double)
                portObject[getKey(JsonKeyID::Value)] = value.toDouble();
            else
                portObject[getKey(JsonKeyID::Value)] = value.toString();

            ports.append(portObject);
        }
        nodeObject[getKey(JsonKeyID::Ports)] = ports;

        nodes.append(nodeObject);
    }
    sceneObject[getKey(JsonKeyID::Nodes)] = nodes;

    QJsonArray connections;
    for (const auto c : m_graphConnections) {
        GraphConnection *conn = static_cast<GraphConnection *>(c);
        QJsonObject connectionObject;
        connectionObject[getKey(JsonKeyID::Source)] = conn->outputPort()->node()->name();
        connectionObject[getKey(JsonKeyID::Output)] = conn->outputPort()->name();
        connectionObject[getKey(JsonKeyID::Target)] = conn->inputPort()->node()->name();
        connectionObject[getKey(JsonKeyID::Input)] = conn->inputPort()->name();
        connections.append(connectionObject);
    }
    sceneObject[getKey(JsonKeyID::Connections)] = connections;

    QJsonDocument doc;
    doc.setObject(sceneObject);

    QTextStream stream(&outputFile);
    stream << doc.toJson();
    return true;
}

bool GraphCore::loadFrom(const QString &fileName)
{
    QFile inputFile(fileName);
    if (!inputFile.open(QFile::ReadOnly | QFile::Text)) {
        emit errorOccurred(tr("Unable to open file '%1'").arg(fileName));
        return false;
    }

    QTextStream stream(&inputFile);
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(stream.readAll().toLocal8Bit(), &err);
    if (err.error != QJsonParseError::ParseError::NoError) {
        emit errorOccurred(err.errorString());
        return false;
    }

    QJsonObject sceneObject = doc.object();
    if (sceneObject.isEmpty()) {
        emit errorOccurred(tr("File '%1' does not contain proper data").arg(fileName));
        return false;
    }


    for (auto conn : m_graphConnections)
        conn->deleteLater();
    m_graphConnections.clear();

    for (auto node : m_graphNodes)
        node->deleteLater();
    m_graphNodes.clear();

    m_zoomFactor = sceneObject.value(getKey(JsonKeyID::ZoomFactor)).toInt(1.0);

    QSignalBlocker block(this);
    const QJsonArray nodes = sceneObject.value(getKey(JsonKeyID::Nodes)).toArray();
    for (const auto &node : nodes) {
        if (!node.isObject()) {
            qWarning() << "Node is not a JSON object";
            continue;
        }
        const QJsonObject &nodeObject = node.toObject();
        const QString &name = nodeObject.value(getKey(JsonKeyID::Name)).toString();
        const qreal x = nodeObject.value(getKey(JsonKeyID::XCoord)).toDouble();
        const qreal y = nodeObject.value(getKey(JsonKeyID::YCoord)).toDouble();
        GraphNode *graphNode = addGraphNode(name, QPointF(x, y));
        if (!graphNode) {
            qWarning() << "Unable to add a new node:" << name;
            continue;
        }
        const QJsonArray &ports = nodeObject.value(getKey(JsonKeyID::Ports)).toArray();
        for (const auto &p : ports) {
            const QJsonObject &port = p.toObject();
            const QString &name = port.value(getKey(JsonKeyID::Name)).toString();
            const int portType = port.value(getKey(JsonKeyID::Type)).toInt();
            const QJsonValue &value = port.value(getKey(JsonKeyID::Value));
            QVariant val;
            if (value.isDouble()) {
                val = value.toDouble();
            }
            else {
                const QString &str = value.toString();
                bool ok = false;
                int intVal = str.toInt(&ok);
                if (ok)
                    val = intVal;
                else
                    val = str;
            }
            if (portType == GraphNodePort::OutputPort)
                graphNode->addOutputPort(name, value);
            else
                graphNode->addInputPort(name, value);
        }
    }

    const QJsonArray connections = sceneObject.value(getKey(JsonKeyID::Connections)).toArray();
    for (const auto &conn : connections) {
        if (!conn.isObject()) {
            qWarning() << "Connection is not a JSON object";
            continue;
        }
        const QJsonObject &connectionObject = conn.toObject();
        const QString &source = connectionObject.value(getKey(JsonKeyID::Source)).toString();
        const QString &out = connectionObject.value(getKey(JsonKeyID::Output)).toString();
        const QString &target = connectionObject.value(getKey(JsonKeyID::Target)).toString();
        const QString &in = connectionObject.value(getKey(JsonKeyID::Input)).toString();
        if (!addGraphConnection(source, out, target, in)) {
            qWarning() << "Unable to add a new connection:" << source << out << target << in;
            continue;
        }
    }

    return true;
}

GraphCore::JsonKeyID GraphCore::getId(const QString &key)
{
    static QHash<QString, GraphCore::JsonKeyID> aliases;
    if (aliases.isEmpty()) {
        for (int i = 0; i < JsonKeyID::END_ID; ++i) {
            const JsonKeyID key = static_cast<JsonKeyID>(i);
            aliases[getKey(key)] = key;
        }
    }
    return aliases.value(key, JsonKeyID::Undefined);
}

QString GraphCore::getKey(GraphCore::JsonKeyID id)
{
    static QVector<QString> aliases;
    if (aliases.isEmpty()) {
        aliases.reserve(JsonKeyID::END_ID);
        aliases << QStringLiteral("name") << QStringLiteral("zoom_factor")
                << QStringLiteral("nodes") << QStringLiteral("connections")
                << QStringLiteral("ports") << QStringLiteral("type")
                << QStringLiteral("x") << QStringLiteral("y") << QStringLiteral("value")
                << QStringLiteral("source") << QStringLiteral("target")
                << QStringLiteral("output") << QStringLiteral("input");
    }
    return aliases.value(id);
}
