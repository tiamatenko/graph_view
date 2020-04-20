#include "graphnode.h"

#include "graphcore.h"
#include "graphnodeport.h"

GraphNode::GraphNode(const QPointF &coord, const QString &name, GraphCore *graphCore)
    : GraphGenericObject(name, graphCore), m_coord(coord)
{
}

GraphCore *GraphNode::graphCore() const
{
    return static_cast<GraphCore *>(parent());
}

GraphNodePort *GraphNode::outputPort(const QString &portName) const
{
    return qobject_cast<GraphNodePort *>(m_outputPorts.value(portName));
}

bool GraphNode::addOutputPort(const QString &portName, const QVariant &value)
{
    if (m_outputPorts.contains(portName)) {
        emit errorOccurred(tr("Output port '%1' already exists").arg(portName));
        return false;
    }
    GraphNodePort *port = new GraphNodePort(GraphNodePort::OutputPort, value, portName, this);
    m_outputPorts[portName] = port;
    emit outputPortsChanged();
    return true;
}

bool GraphNode::removeOutputPort(const QString &portName)
{
    auto it = m_outputPorts.find(portName);
    if (it == m_outputPorts.end()) {
        emit errorOccurred(tr("Output port '%1' does not exist").arg(portName));
        return false;
    }
    QObject *obj = it.value();
    m_outputPorts.erase(it);
    emit outputPortsChanged();
    obj->deleteLater();
    return true;
}

GraphNodePort *GraphNode::inputPort(const QString &portName) const
{
    return qobject_cast<GraphNodePort *>(m_inputPorts.value(portName));
}

bool GraphNode::addInputPort(const QString &portName, const QVariant &value)
{
    if (m_inputPorts.contains(portName)) {
        emit errorOccurred(tr("Input port '%1' already exists").arg(portName));
        return false;
    }
    GraphNodePort *port = new GraphNodePort(GraphNodePort::InputPort, value, portName, this);
    m_inputPorts[portName] = port;
    emit inputPortsChanged();
    return true;
}

bool GraphNode::removeInputPort(const QString &portName)
{
    auto it = m_inputPorts.find(portName);
    if (it == m_inputPorts.end()) {
        emit errorOccurred(tr("Input port '%1' does not exist").arg(portName));
        return false;
    }
    QObject *obj = it.value();
    m_inputPorts.erase(it);
    emit inputPortsChanged();
    obj->deleteLater();
    return true;
}
