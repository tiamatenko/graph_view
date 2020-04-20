#include "graphnode.h"

#include "graphnodeport.h"
#include "graphcore.h"

#include <QDebug>

GraphNodePort::GraphNodePort(PortType portType, const QVariant &value, const QString &name, GraphNode *parent)
    : GraphGenericObject(name, parent)
    , m_portType(portType)
    , m_value(value)
{
    qDebug() << m_value.type();
    switch (m_value.type()) {
    case QVariant::Int:
        m_color = Qt::red;
        break;
    case QVariant::Double:
        m_color = Qt::green;
        break;
    case QVariant::String:
        m_color = Qt::blue;
        break;
    default:
        m_color = Qt::gray;
        break;
    }
}

GraphNode *GraphNodePort::node() const
{
    return static_cast<GraphNode *>(parent());
}

QString GraphNodePort::nodeName() const
{
    return node()->name();
}

bool GraphNodePort::isConnected() const
{
    return node()->graphCore()->hasConnection(this);
}
