#pragma once

#include <QObject>

#include <QHash>
#include <QUrl>

class GraphNode;
class GraphConnection;
class GraphNodePort;

class GraphCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl sourceFileName READ sourceFileName NOTIFY sourceFileNameChanged)
    Q_PROPERTY(QObjectList graphNodes READ graphNodes NOTIFY graphNodesChanged)
    Q_PROPERTY(QObjectList graphConnections READ graphConnections NOTIFY graphConnectionsChanged)
    Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)

    enum JsonKeyID {
        Undefined = -1,
        Name = 0, ZoomFactor, Nodes, Connections, Ports, Type,
        XCoord, YCoord, Value, Source, Target, Output, Input, END_ID
    };
public:
    explicit GraphCore(QObject *parent = nullptr);

    inline QUrl sourceFileName() const { return m_sourceFileName; }
    inline QObjectList graphNodes() const { return m_graphNodes.values(); }
    inline QObjectList graphConnections() const { return m_graphConnections.values(); }
    inline double zoomFactor() const { return m_zoomFactor; }

    GraphNode *findNode(const QString &name) const;
    bool hasConnection(const GraphNodePort *graphNodePort) const;

public slots:
    void save();
    void saveAs(const QUrl &fileName);
    void load(const QUrl &fileName);
    void clear();

    bool addGraphNode(const QString &name, qreal x, qreal y);
    bool removeGraphNode(const QString &name);

    bool addGraphConnection(const QString &src, const QString &out, const QString &dest, const QString &in);
    bool removeGraphConnection(const QString &name);

    void setZoomFactor(double zoomFactor);

signals:
    void sourceFileNameChanged();
    void zoomFactorChanged();
    void graphNodesChanged();
    void graphConnectionsChanged();
    void errorOccurred(const QString &error);

protected:
    bool saveTo(const QUrl &fileName);
    bool loadFrom(const QUrl &fileName);

    static JsonKeyID getId(const QString &key);
    static QString getKey(JsonKeyID id);

private:
    QUrl m_sourceFileName;
    double m_zoomFactor = 1.0;
    QHash<QString, QObject *> m_graphNodes;
    QHash<QString, QObject *> m_graphConnections;    
};
