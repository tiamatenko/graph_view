#pragma once

#include "graphgenericobject.h"

#include <QPointer>

class GraphNodePort;

class GraphConnection : public GraphGenericObject
{
    Q_OBJECT
    Q_PROPERTY(GraphNodePort *outputPort READ outputPort CONSTANT)
    Q_PROPERTY(GraphNodePort *inputPort READ inputPort CONSTANT)

public:
    explicit GraphConnection(GraphNodePort *outPort, GraphNodePort *inPort, const QString &name, QObject *parent);

    GraphNodePort *outputPort() const;
    GraphNodePort *inputPort() const;

signals:

private:
    QPointer<GraphNodePort> m_outputPort;
    QPointer<GraphNodePort> m_inputPort;
};

