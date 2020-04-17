#include "graphnodeport.h"

GraphNodePort::GraphNodePort(PortType portType, const QVariant &value, const QString &name, QObject *parent)
    : GraphGenericObject(name, parent)
    , m_portType(portType)
    , m_value(value)
{
    switch (m_value.type()) {
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Char:
        m_color = Qt::red;
        break;
    case QVariant::Double:
        m_color = Qt::green;
        break;
    case QVariant::String:
        m_color = Qt::blue;
        break;
    default:
        m_color = Qt::gray;
        break;
    }
}
