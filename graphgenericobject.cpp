#include "graphgenericobject.h"

GraphGenericObject::GraphGenericObject(const QString &name, QObject *parent)
    : QObject(parent), m_name(name)
{
}
