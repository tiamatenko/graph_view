#include "graphnode.h"

#include "graphnodeport.h"

GraphNode::GraphNode(const QPointF &coord, const QString &name, QObject *parent)
    : GraphGenericObject(name, parent), m_coord(coord)
{
}

GraphNodePort *GraphNode::outputPort(const QString &portName) const
{
    return qobject_cast<GraphNodePort *>(m_outputPorts.value(portName));
}

void GraphNode::addOutputPort(const QString &portName, const QVariant &value)
{
    if (m_outputPorts.contains(portName)) {
        emit errorOccurred(tr("Output port '%1' already exists").arg(portName));
        return;
    }
    GraphNodePort *port = new GraphNodePort(GraphNodePort::OutputPort, value, portName, this);
    m_outputPorts[portName] = port;
    emit outputPortsChanged();
}

void GraphNode::removeOutputPort(const QString &portName)
{
    auto it = m_outputPorts.find(portName);
    if (it == m_outputPorts.end()) {
        emit errorOccurred(tr("Output port '%1' does not exist").arg(portName));
        return;
    }
    QObject *obj = it.value();
    m_outputPorts.erase(it);
    emit outputPortsChanged();
    obj->deleteLater();
}

GraphNodePort *GraphNode::inputPort(const QString &portName) const
{
    return qobject_cast<GraphNodePort *>(m_inputPorts.value(portName));
}

void GraphNode::addInputPort(const QString &portName, const QVariant &value)
{
    if (m_inputPorts.contains(portName)) {
        emit errorOccurred(tr("Input port '%1' already exists").arg(portName));
        return;
    }
    GraphNodePort *port = new GraphNodePort(GraphNodePort::InputPort, value, portName, this);
    m_inputPorts[portName] = port;
    emit inputPortsChanged();
}

void GraphNode::removeInputPort(const QString &portName)
{
    auto it = m_inputPorts.find(portName);
    if (it == m_inputPorts.end()) {
        emit errorOccurred(tr("Input port '%1' does not exist").arg(portName));
        return;
    }
    QObject *obj = it.value();
    m_inputPorts.erase(it);
    emit inputPortsChanged();
    obj->deleteLater();
}
