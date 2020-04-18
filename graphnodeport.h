#pragma once

#include "graphgenericobject.h"

#include <QVariant>

class GraphNode;

class GraphNodePort : public GraphGenericObject
{
    Q_OBJECT
    Q_PROPERTY(PortType portType READ portType CONSTANT)
    Q_PROPERTY(QVariant value READ value CONSTANT)

public:
    enum PortType { OutputPort, InputPort };
    Q_ENUM(PortType)

    explicit GraphNodePort(PortType portType, const QVariant &value, const QString &name, GraphNode *parentNode);

    inline PortType portType() const { return m_portType; }
    inline QVariant value() const { return m_value; }

    GraphNode *node() const;

private:
    const PortType m_portType;
    QVariant m_value;
};
