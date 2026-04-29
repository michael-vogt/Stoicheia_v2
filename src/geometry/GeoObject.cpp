#include "GeoObject.h"

#include "UpdateGuard.h"

GeoObject::~GeoObject() {
    detach();
}

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

// cut connections - for Scene::remove()
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

void GeoObject::onSourceRemoved(GeoObject *src) {
    m_valid = false;
}

bool GeoObject::isValid() const {
    return m_valid;
}

std::unordered_set<GeoObject *> GeoObject::dependents() {
    return m_dependents;
}

/*void GeoObject::enqueueTransitive() const {
    for (GeoObject* dep : m_dependents) {
        if (!UpdateGuard::isPending(dep)) {
            UpdateGuard::enqueue(dep);
            dep->enqueueTransitive();
        }
    }
}

void GeoObject::notify() {
    if (UpdateGuard::isActive()) {
        UpdateGuard::enqueue(this);
        enqueueTransitive();
    } else {
        std::unordered_set<GeoObject*> visited;
        notifyDependents(visited);
    }
}*/

/*void GeoObject::notifyDependents(std::unordered_set<GeoObject *> &visited) const {
    for (GeoObject* dep : m_dependents) {
        if (visited.contains(dep)) continue;
        visited.insert(dep);
        dep->recompute();
        dep->notifyDependents(visited);
    }
}*/
