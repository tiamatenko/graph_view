#pragma once

#include <QObject>

#include <QHash>

class GraphCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFileName READ sourceFileName NOTIFY sourceFileNameChanged)
    Q_PROPERTY(QObjectList graphObjects READ graphObjects NOTIFY graphObjectsChanged)
    Q_PROPERTY(QObjectList graphConnections READ graphConnections NOTIFY graphConnectionsChanged)
    Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor)

public:
    explicit GraphCore(QObject *parent = nullptr);

    Q_INVOKABLE void save() const;
    Q_INVOKABLE void saveAs(const QString &fileName);
    Q_INVOKABLE void load(const QString &fileName);

    Q_INVOKABLE void addGraphObject(const QString &name);
    Q_INVOKABLE void removeGraphObject(const QString &name);

    Q_INVOKABLE void addGraphConnection(const QString &src, const QString &out, const QString &dest, const QString &in);
    Q_INVOKABLE void removeGraphConnection(const QString &name);

    inline QString sourceFileName() const { return m_sourceFileName; }
    inline double zoomFactor() const { return m_zoomFactor; }
    inline QObjectList graphObjects() const { return m_graphObjects.values(); }
    inline QObjectList graphConnections() const { return m_graphConnections.values(); }

public slots:
    void setZoomFactor(double zoomFactor);

signals:
    void sourceFileNameChanged(const QString &sourceFileName);
    void graphObjectsChanged();
    void graphConnectionsChanged();
    void errorOccurred(const QString &error);

    void zoomFactorChanged(double zoomFactor);

protected:
    bool saveTo(const QString &fileName) const;

private:
    QString m_sourceFileName;
    QHash<QString, QObject*> m_graphObjects;
    QHash<QString, QObject*> m_graphConnections;
    double m_zoomFactor = 1.0;
};
