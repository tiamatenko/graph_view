#pragma once

#include "graphgenericobject.h"

#include <QHash>
#include <QPointF>

class GraphCore;
class GraphNodePort;

class GraphNode : public GraphGenericObject
{
    Q_OBJECT
    Q_PROPERTY(qreal xCoord READ xCoord WRITE setXCoord)
    Q_PROPERTY(qreal yCoord READ yCoord WRITE setYCoord)
    Q_PROPERTY(QObjectList outputPorts READ outputPorts NOTIFY outputPortsChanged)
    Q_PROPERTY(QObjectList inputPorts READ inputPorts NOTIFY inputPortsChanged)

public:
    explicit GraphNode(const QPointF &coord, const QString &name, GraphCore *graphCore);

    GraphCore *graphCore() const;

    inline QPointF coord() const { return m_coord; }
    inline qreal xCoord() const { return m_coord.x(); }
    inline qreal yCoord() const { return m_coord.y(); }

    inline QObjectList outputPorts() const { return m_outputPorts.values(); }
    inline QObjectList inputPorts() const { return m_inputPorts.values(); }

    void clearPorts();

public slots:
    inline void setXCoord(double xCoord) { m_coord.setX(xCoord); }
    inline void setYCoord(double yCoord) { m_coord.setY(yCoord); }

    GraphNodePort *outputPort(const QString &portName) const;
    bool addOutputPort(const QString &portName, const QVariant &value);
    bool removeOutputPort(const QString &portName);

    GraphNodePort *inputPort(const QString &portName) const;
    bool addInputPort(const QString &portName, const QVariant &value);
    bool removeInputPort(const QString &portName);

signals:
    void outputPortsChanged();
    void inputPortsChanged();

private:
    QPointF m_coord;
    QHash<QString, QObject *> m_outputPorts;
    QHash<QString, QObject *> m_inputPorts;
};

