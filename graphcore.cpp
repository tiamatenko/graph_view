#include "graphcore.h"


/**
 * @brief The GraphCore class owns and managers all objects of the graph view.
 * Also it can create and remove objects and conntions. save to and load from file
*/

/**
 * @brief GraphCore::GraphCore ctor
 * @param parent
 */
GraphCore::GraphCore(QObject *parent)
    : QObject(parent)
    , m_sourceFileName(tr("<Empty>"))
{

}

/**
 * @brief GraphCore::save saves all objects to the source file
 */
void GraphCore::save() const
{
    saveTo(sourceFileName());
}

/**
 * @brief GraphCore::saveAs saves all objects to a new file
 * @param fileName new file name
 */
void GraphCore::saveAs(const QString &fileName)
{
    if (!saveTo(sourceFileName()))
        return;

    if (sourceFileName() != fileName) {
        m_sourceFileName = fileName;
        emit sourceFileNameChanged(m_sourceFileName);
    }
}

/**
 * @brief GraphCore::load loads data from file
 * @param fileName a file name
 */
void GraphCore::load(const QString &fileName)
{
    Q_UNUSED(fileName)

    // TODO
}

/**
 * @brief GraphCore::addGraphObject creates a new object
 * @param name of a new object
 */
void GraphCore::addGraphObject(const QString &name)
{
    if (m_graphObjects.contains(name)) {
        emit errorOccurred(tr("Graph object with name '%1' already exists").arg(name));
        return;
    }
    QObject *obj = new QObject(this);  // TODO
    obj->setObjectName(name);
    m_graphObjects[name] = obj;
    emit graphObjectsChanged();
}

/**
 * @brief GraphCore::removeGraphObject removes object
 * @param name of removed object
 */
void GraphCore::removeGraphObject(const QString &name)
{
    auto it = m_graphObjects.find(name);
    if (it == m_graphObjects.end()) {
        emit errorOccurred(tr("Graph object with name '%1' does not exist").arg(name));
        return;
    }
    QObject *obj = it.value();
    m_graphObjects.erase(it);
    emit graphObjectsChanged();
    obj->deleteLater();
}

/**
 * @brief GraphCore::addGraphConnection creates a new connection
 * @param src name of the source object
 * @param out output port name of the source object
 * @param dest name of the destination object
 * @param in input port name of the destination object
 */
void GraphCore::addGraphConnection(const QString &src, const QString &out, const QString &dest, const QString &in)
{
    const QString name = QString(QLatin1String("%1.%2->%3.%4")).arg(src, out, dest, in);
    if (m_graphConnections.contains(name)) {
        emit errorOccurred(tr("Connection %1 already exists").arg(name));
        return;
    }
    QObject *obj = new QObject(this);   // TODO
    obj->setObjectName(name);
    m_graphConnections[name] = obj;
    emit graphConnectionsChanged();
}

/**
 * @brief GraphCore::removeGraphConnection removes connection
 * @param name of connection
 */
void GraphCore::removeGraphConnection(const QString &name)
{
    auto it = m_graphConnections.find(name);
    if (it == m_graphConnections.end()) {
        emit errorOccurred(tr("Connection '%1' does not exist").arg(name));
        return;
    }
    QObject *obj = it.value();
    m_graphConnections.erase(it);
    emit graphConnectionsChanged();
    obj->deleteLater();
}

/**
 * @brief GraphCore::setZoomFactor sets a new zoom factor
 * @param zoomFactor value of a new zoom factor
 */
void GraphCore::setZoomFactor(double zoomFactor)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_zoomFactor, zoomFactor))
        return;

    m_zoomFactor = zoomFactor;
}

/**
 * @brief GraphCore::saveTo saves all current data to file
 * @param fileName file name
 * @return result of saving
 */
bool GraphCore::saveTo(const QString &fileName) const
{
    Q_UNUSED(fileName)
    // TODO
    return false;
}
