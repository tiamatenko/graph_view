#include "graphconnection.h"
#include "graphcore.h"
#include "graphcore.h"
#include "graphnode.h"


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
void GraphCore::save() const
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
    Q_UNUSED(fileName)

    // TODO
}

/**
 * @brief GraphCore::addGraphNode creates a new node
 * @param name of a new node
 */
void GraphCore::addGraphNode(const QPointF &coord, const QString &name)
{
    if (m_graphNodes.contains(name)) {
        emit errorOccurred(tr("Graph node '%1' already exists").arg(name));
        return;
    }
    GraphNode *node = new GraphNode(coord, name, this);
    m_graphNodes[name] = node;

    connect(node, &GraphNode::outputPortsChanged, this, &GraphCore::graphChanged);
    connect(node, &GraphNode::inputPortsChanged, this, &GraphCore::graphChanged);
    connect(node, &GraphNode::errorOccurred, this, &GraphCore::errorOccurred);
    emit graphChanged();
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
void GraphCore::addGraphConnection(const QString &src, const QString &out, const QString &dest, const QString &in)
{
    const QString name = QString(QLatin1String("%1.%2->%3.%4")).arg(src, out, dest, in);
    if (m_graphConnections.contains(name)) {
        emit errorOccurred(tr("Connection '%1' already exists").arg(name));
        return;
    }
    GraphNode *sourceNode = qobject_cast<GraphNode *>(m_graphNodes.value(src));
    if (!sourceNode) {
        emit errorOccurred(tr("Unable to find '%1' node").arg(src));
        return;
    }
    GraphNode *destNode = qobject_cast<GraphNode *>(m_graphNodes.value(dest));
    if (!destNode) {
        emit errorOccurred(tr("Unable to find '%1' node").arg(dest));
        return;
    }
    GraphNodePort *outPort = sourceNode->outputPort(out);
    if (!outPort) {
        emit errorOccurred(tr("Unable to find '%1' port").arg(out));
        return;
    }
    GraphNodePort *inPort = destNode->inputPort(in);
    if (!inPort) {
        emit errorOccurred(tr("Unable to find '%1' port").arg(in));
        return;
    }

    GraphConnection *conn = new GraphConnection(outPort, inPort, name, this);
    m_graphConnections[name] = conn;
    connect(conn, &GraphConnection::errorOccurred, this, &GraphCore::errorOccurred);
    emit graphChanged();
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
bool GraphCore::saveTo(const QString &fileName) const
{
    Q_UNUSED(fileName)
    // TODO
    return false;
}
