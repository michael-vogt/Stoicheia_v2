#include "MergePointsCommand.h"
#include "Structs.h"

MergePointsCommand::MergePointsCommand(SceneAdapter *adapter, PointPairForMergePointCommand points)
    : m_adapter(adapter), m_survivor(points.survivor), m_absorbed(points.absorbed)
{}

void MergePointsCommand::execute() {
    m_absorbedX = m_absorbed->x();
    m_absorbedY = m_absorbed->y();
    m_rewired.clear();

    std::vector<GeoObject*> deps(m_absorbed->dependents().begin(), m_absorbed->dependents().end());

    for (GeoObject* dep : deps) {
        m_absorbed->removeDependent(dep);
        m_survivor->addDependent(dep);
        dep->replaceSource(GeoObjectPair{.oldGeoObject=m_absorbed, .newGeoObject=m_survivor});
        dep->recompute();
        m_rewired.push_back(dep);
    }

    m_adapter->removeGraphicsOnly(m_absorbed);
}

void MergePointsCommand::undo() {
    for (GeoObject* dep : m_rewired) {
        if (!m_survivor->dependents().contains(dep)) {
            continue;
        }
        m_survivor->removeDependent(dep);
        m_absorbed->addDependent(dep);
        dep->replaceSource(GeoObjectPair{.oldGeoObject=m_survivor, .newGeoObject=m_absorbed});
        dep->recompute();
    }

    m_absorbed->moveTo(m_absorbedX, m_absorbedY);
    m_adapter->addPoint(m_absorbed);
}