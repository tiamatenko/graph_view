#pragma once

#include <QObject>

#include <QHash>

class GraphNode;
class GraphConnection;
class GraphNodePort;

class GraphCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFileName READ sourceFileName NOTIFY sourceFileNameChanged)
    Q_PROPERTY(QObjectList graphNodes READ graphNodes NOTIFY graphChanged)
    Q_PROPERTY(QObjectList graphConnections READ graphConnections NOTIFY graphChanged)
    Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor)

    enum JsonKeyID {
        Undefined = -1,
        Name = 0, ZoomFactor, Nodes, Connections, Ports, Type,
        XCoord, YCoord, Value, Source, Target, Output, Input, END_ID
    };
public:
    explicit GraphCore(QObject *parent = nullptr);

    inline QString sourceFileName() const { return m_sourceFileName; }
    inline QObjectList graphNodes() const { return m_graphNodes.values(); }
    inline QObjectList graphConnections() const { return m_graphConnections.values(); }
    inline double zoomFactor() const { return m_zoomFactor; }

    GraphNode *findNode(const QString &name) const;
    bool hasConnection(const GraphNodePort *graphNodePort) const;

public slots:
    void save();
    void saveAs(const QString &fileName);
    void load(const QString &fileName);

    bool addGraphNode(const QString &name, qreal x, qreal y);
    bool removeGraphNode(const QString &name);

    bool addGraphConnection(const QString &src, const QString &out, const QString &dest, const QString &in);
    bool removeGraphConnection(const QString &name);

    void setZoomFactor(double zoomFactor);

signals:
    void sourceFileNameChanged(const QString &sourceFileName);
    void zoomFactorChanged(double zoomFactor);
    void graphChanged();
    void errorOccurred(const QString &error);

protected:
    bool saveTo(const QString &fileName);
    bool loadFrom(const QString &fileName);

    static JsonKeyID getId(const QString &key);
    static QString getKey(JsonKeyID id);

private:
    QString m_sourceFileName;
    double m_zoomFactor = 1.0;
    QHash<QString, QObject *> m_graphNodes;
    QHash<QString, QObject *> m_graphConnections;    
};
