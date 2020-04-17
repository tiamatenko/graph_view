#pragma once

#include "graphgenericobject.h"

#include <QHash>
#include <QPointF>

class GraphNode : public GraphGenericObject
{
    Q_OBJECT
    Q_PROPERTY(qreal xCoord READ xCoord WRITE setXCoord)
    Q_PROPERTY(qreal yCoord READ yCoord WRITE setYCoord)
    Q_PROPERTY(QObjectList outputPorts READ outputPorts NOTIFY outputPortsChanged)
    Q_PROPERTY(QObjectList inputPorts READ inputPorts NOTIFY inputPortsChanged)

public:
    explicit GraphNode(const QPointF &coords, const QString &name, QObject *parent);

    inline QPointF coords() const { return m_coords; }
    inline qreal xCoord() const { return m_coords.x(); }
    inline qreal yCoord() const { return m_coords.y(); }

    inline QObjectList outputPorts() const { return m_outputPorts.values(); }
    inline QObjectList inputPorts() const { return m_inputPorts.values(); }

public slots:
    inline void setXCoord(double xCoord) { m_coords.setX(xCoord); }
    inline void setYCoord(double yCoord) { m_coords.setY(yCoord); }

    void addOutputPort(const QString &name);
    void removeOutputPort(const QString &name);

    void addInputPort(const QString &name);
    void removeInputPort(const QString &name);

signals:
    void outputPortsChanged();
    void inputPortsChanged();

private:
    QPointF m_coords;
    QHash<QString, QObject*> m_outputPorts;
    QHash<QString, QObject*> m_inputPorts;
};

