#include "graphconnection.h"

#include "graphcore.h"
#include "graphnodeport.h"

GraphConnection::GraphConnection(GraphNodePort *outPort, GraphNodePort *inPort, const QString &name, GraphCore *graphCore)
    : GraphGenericObject(name, graphCore)
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

QString GraphConnection::sourceNodeName() const
{
    return outputPort()->nodeName();
}

QString GraphConnection::outputPortName() const
{
    return outputPort()->name();
}

QString GraphConnection::targetNodeName() const
{
    return inputPort()->nodeName();
}

QString GraphConnection::inputPortName() const
{
    return inputPort()->name();
}
