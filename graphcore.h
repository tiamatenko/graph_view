#pragma once

#include <QObject>

#include <QHash>

class GraphCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFileName READ sourceFileName NOTIFY sourceFileNameChanged)
    Q_PROPERTY(QObjectList graphNodes READ graphNodes NOTIFY graphChanged)
    Q_PROPERTY(QObjectList graphConnections READ graphConnections NOTIFY graphChanged)
    Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor)

public:
    explicit GraphCore(QObject *parent = nullptr);

    inline QString sourceFileName() const { return m_sourceFileName; }
    inline QObjectList graphNodes() const { return m_graphNodes.values(); }
    inline QObjectList graphConnections() const { return m_graphConnections.values(); }
    inline double zoomFactor() const { return m_zoomFactor; }

public slots:
    void save() const;
    void saveAs(const QString &fileName);
    void load(const QString &fileName);

    void addGraphNode(const QPointF &coord, const QString &name);
    void removeGraphNode(const QString &name);

    void addGraphConnection(const QString &src, const QString &out, const QString &dest, const QString &in);
    void removeGraphConnection(const QString &name);

    void setZoomFactor(double zoomFactor);

signals:
    void sourceFileNameChanged(const QString &sourceFileName);
    void graphChanged();
    void errorOccurred(const QString &error);

    void zoomFactorChanged(double zoomFactor);

protected:
    bool saveTo(const QString &fileName) const;

private:
    QString m_sourceFileName;
    QHash<QString, QObject *> m_graphNodes;
    QHash<QString, QObject *> m_graphConnections;
    double m_zoomFactor = 1.0;
};
