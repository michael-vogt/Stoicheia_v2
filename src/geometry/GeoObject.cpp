#include "GeoObject.h"

#include "UpdateGuard.h"

void GeoObject::addDependent(GeoObject *dep) {
    if (dep == nullptr) return;
    m_dependents.insert(dep);
    dep->m_sources.insert(this); // backward reference
}

void GeoObject::removeDependent(GeoObject *dep) {
    if (dep == nullptr) return;
    m_dependents.erase(dep);
    dep->m_sources.erase(this);
}

void GeoObject::detach() {
    updateGuardDequeue(this);

    for (GeoObject* src : m_sources) {
        src->m_dependents.erase(this);
    }
    m_sources.clear();

    for (GeoObject* dep : m_dependents) {
        dep->m_sources.erase(this);
        dep->onSourceRemoved(this);
    }
    m_dependents.clear();
}
