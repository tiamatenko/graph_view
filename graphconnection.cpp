#include "graphconnection.h"

#include "graphnodeport.h"

GraphConnection::GraphConnection(GraphNodePort *outPort, GraphNodePort *inPort, const QString &name, QObject *parent)
    : GraphGenericObject(name, parent)
    , m_outputPort(outPort)
    , m_inputPort(inPort)
{
    m_color = outPort->color();
}
