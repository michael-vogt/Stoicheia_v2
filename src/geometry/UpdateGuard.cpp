#include "UpdateGuard.h"

#include <functional>
#include <queue>
#include <stdexcept>
#include <unordered_map>

void updateGuardDequeue(class GeoObject *obj) {
    UpdateGuard::dequeue(obj);
}

int UpdateGuard::depth = 0;
bool UpdateGuard::flushing = false;
std::unordered_set<GeoObject*> UpdateGuard::pending;

// ── GeoObject::notify & helpers ──────────────────────────────────────────────

void GeoObject::enqueueTransitive() const {
    for (GeoObject* dep : m_dependents) {
        if (!UpdateGuard::isPending(dep)) {
            UpdateGuard::enqueue(dep);
            dep->enqueueTransitive();
        }
    }
}

void GeoObject::notifyDirect() {
    std::unordered_set<GeoObject*> visited;
    std::function<void(GeoObject*)> propagate = [&](GeoObject* obj) {
        for (GeoObject* dep : obj->dependents()) {
            if (visited.contains(dep)) continue;
            visited.insert(dep);
            dep->recompute();
            propagate(dep);
        }
    };
    propagate(this);
}

void GeoObject::notify() {
    if (UpdateGuard::isActive()) {
        UpdateGuard::enqueue(this);
        enqueueTransitive();
    } else {
        notifyDirect();
    }
}


UpdateGuard::UpdateGuard() {
    ++depth;
}

UpdateGuard::~UpdateGuard() {
    if (--depth == 0) flush();
}

void UpdateGuard::enqueue(GeoObject *obj) {
    pending.insert(obj);
}

void UpdateGuard::dequeue(GeoObject *obj) {
    pending.erase(obj);
}

void UpdateGuard::flush() {
    std::unordered_map<GeoObject*, int> inDegree;
    for (GeoObject* obj : pending) {
        inDegree[obj] = 0;
    }

    for (GeoObject* obj : pending) {
        for (GeoObject* dep : obj->dependents()) {
            if (pending.contains(dep)) {
                inDegree[dep]++;
            }
        }
    }

    std::queue<GeoObject*> ready;
    for (auto& [obj, degree] : inDegree) {
        if (degree == 0) ready.push(obj);
    }

    std::vector<GeoObject*> order;
    while (!ready.empty()) {
        GeoObject* obj = ready.front();
        ready.pop();
        order.push_back(obj);

        for (GeoObject* dep : obj->dependents()) {
            if (!pending.contains(dep)) continue;
            if (--inDegree[dep] == 0) {
                ready.push(dep);
            }
        }
    }

    if (order.size() != pending.size()) {
        throw std::runtime_error("Zyklus im Abhängigkeitsgraph!");
    }

    pending.clear();
    for (GeoObject* obj : order) {
        if (obj->isValid()) {
            obj->recompute();
        }
    }
}

bool UpdateGuard::isActive() {
    return depth > 0;
}

bool UpdateGuard::isPending(GeoObject* obj) {
    return pending.contains(obj);
}
