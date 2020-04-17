#pragma once

#include <QObject>
#include <QColor>

class GraphGenericObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QColor color READ color)
public:
    explicit GraphGenericObject(const QString &name, QObject *parent);

    inline QString name() const { return m_name; }
    inline QColor color() const { return m_color; }

signals:
    void errorOccurred(const QString &error);

protected:
    QString m_name;
    QColor m_color;
};

