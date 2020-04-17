#include "graphconnection.h"

#include "graphnodeport.h"

GraphConnection::GraphConnection(GraphNodePort *outPort, GraphNodePort *inPort, const QString &name, QObject *parent)
    : GraphGenericObject(name, parent)
    , m_outputPort(outPort)
    , m_inputPort(inPort)
{
    m_color = outPort->color();
}

GraphNodePort *GraphConnection::outputPort() const
{
    return m_outputPort;
}

GraphNodePort *GraphConnection::inputPort() const
{
    return m_inputPort;
}
