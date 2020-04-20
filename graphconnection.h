#pragma once

#include "graphgenericobject.h"

#include <QPointer>

class GraphCore;
class GraphNodePort;

class GraphConnection : public GraphGenericObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceNodeName READ sourceNodeName CONSTANT)
    Q_PROPERTY(QString outputPortName READ outputPortName CONSTANT)
    Q_PROPERTY(QString targetNodeName READ targetNodeName CONSTANT)
    Q_PROPERTY(QString inputPortName READ inputPortName CONSTANT)

public:
    explicit GraphConnection(GraphNodePort *outPort, GraphNodePort *inPort, const QString &name, GraphCore *graphCore);

    GraphNodePort *outputPort() const;
    GraphNodePort *inputPort() const;

    QString sourceNodeName() const;
    QString outputPortName() const;
    QString targetNodeName() const;
    QString inputPortName() const;

private:
    QPointer<GraphNodePort> m_outputPort;
    QPointer<GraphNodePort> m_inputPort;
};

