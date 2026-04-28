#include "UpdateGuard.h"

#include <queue>
#include <stdexcept>
#include <unordered_map>

UpdateGuard::UpdateGuard() {
    ++depth;
}

UpdateGuard::~UpdateGuard() {
    if (--depth == 0) flush();
}

void UpdateGuard::enqueue(GeoObject *obj) {
    pending.insert(obj);
}

int UpdateGuard::depth = 0;
std::unordered_set<GeoObject*> UpdateGuard::pending;

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
