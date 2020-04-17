#pragma once

#include "graphgenericobject.h"

class GraphNodePort;

class GraphConnection : public GraphGenericObject
{
    Q_OBJECT
    Q_PROPERTY(GraphNodePort *outputPort READ outputPort CONSTANT)
    Q_PROPERTY(GraphNodePort *inputPort READ inputPort CONSTANT)

public:
    explicit GraphConnection(GraphNodePort *outPort, GraphNodePort *inPort, const QString &name, QObject *parent);

    inline GraphNodePort *outputPort() const { return m_outputPort; }
    inline GraphNodePort *inputPort() const { return m_inputPort; }

signals:

private:
    GraphNodePort *m_outputPort;
    GraphNodePort *m_inputPort;
};

