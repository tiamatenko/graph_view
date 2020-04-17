#include "graphnode.h"

GraphNode::GraphNode(const QPointF &coords, const QString &name, QObject *parent)
    : GraphGenericObject(name, parent), m_coords(coords)
{

}

void GraphNode::addOutputPort(const QString &name)
{
    if (m_outputPorts.contains(name)) {
        emit errorOccurred(tr("Output port with name '%1' already exists").arg(name));
        return;
    }
    QObject *obj = new QObject(this);  // TODO
    obj->setObjectName(name);
    m_outputPorts[name] = obj;
    emit outputPortsChanged();
}

void GraphNode::removeOutputPort(const QString &name)
{
    auto it = m_outputPorts.find(name);
    if (it == m_outputPorts.end()) {
        emit errorOccurred(tr("Output port with name '%1' does not exist").arg(name));
        return;
    }
    QObject *obj = it.value();
    m_outputPorts.erase(it);
    emit outputPortsChanged();
    obj->deleteLater();
}

void GraphNode::addInputPort(const QString &name)
{
    if (m_inputPorts.contains(name)) {
        emit errorOccurred(tr("Input port with name '%1' already exists").arg(name));
        return;
    }
    QObject *obj = new QObject(this);  // TODO
    obj->setObjectName(name);
    m_inputPorts[name] = obj;
    emit inputPortsChanged();
}

void GraphNode::removeInputPort(const QString &name)
{
    auto it = m_inputPorts.find(name);
    if (it == m_inputPorts.end()) {
        emit errorOccurred(tr("Output port with name '%1' does not exist").arg(name));
        return;
    }
    QObject *obj = it.value();
    m_inputPorts.erase(it);
    emit inputPortsChanged();
    obj->deleteLater();
}
