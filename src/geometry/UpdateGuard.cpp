#include "UpdateGuard.h"

#include <functional>
#include <queue>
#include <stdexcept>
#include <map>
#include <iostream>

void updateGuardDequeue(class GeoObject *obj) {
    UpdateGuard::dequeue(obj);
}

int UpdateGuard::depth = 0;
bool UpdateGuard::flushing = false;

UpdateGuard::~UpdateGuard() { // NOLINT
    if (--depth == 0) {
        try {
            flush();
        } catch (const std::runtime_error& e) {
            // Log the error, but don't throw from the destructor
            std::cerr << "Error during UpdateGuard flush: " << e.what();
        }
    }
}

auto UpdateGuard::pending() -> std::set<GeoObject*>& {
    static std::set<GeoObject*> instance;
    return instance;
}


void UpdateGuard::flush() {
    std::map<GeoObject*, int> in_degree;
    for (GeoObject* obj : pending()) {
        in_degree[obj] = 0;
    }

    for (GeoObject* obj : pending()) {
        for (GeoObject* dep : obj->dependents()) {
            if (pending().contains(dep)) {
                in_degree[dep]++;
            }
        }
    }

    std::queue<GeoObject*> ready;
    for (auto& [obj, degree] : in_degree) {
        if (degree == 0) {
            ready.push(obj);
        }
    }

    std::vector<GeoObject*> order;
    while (!ready.empty()) {
        GeoObject* obj = ready.front();
        ready.pop();
        order.push_back(obj);

        for (GeoObject* dep : obj->dependents()) {
            if (!pending().contains(dep)) {
                continue;
            }
            if (--in_degree[dep] == 0) {
                ready.push(dep);
            }
        }
    }

    if (order.size() != pending().size()) {
        throw std::runtime_error("Zyklus im Abhängigkeitsgraph!");
    }

    pending().clear();
    flushing = true;
    for (GeoObject* obj : order) {
        if (obj->isValid()) {
            obj->recompute();
        }
    }
    flushing = false;
}

// ── GeoObject::notify & helpers ──────────────────────────────────────────────
void GeoObject::enqueueTransitive() const {
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
        notifyDirect();
    }
}

void GeoObject::notifyDirect() {
    std::set<GeoObject*> visited;
    std::function<void(GeoObject*)> propagate = [&](GeoObject* obj) -> void {
        for (GeoObject* dep : obj->dependents()) {
            if (visited.contains(dep)) {
                continue;
            }
            visited.insert(dep);
            dep->recompute();
            propagate(dep);
        }
    };
    propagate(this);
}