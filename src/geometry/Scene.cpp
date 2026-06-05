#include "Scene.h"

#include <algorithm>
#include <functional>

void Scene::remove(GeoObject* target) {
    target->detach();
    std::erase_if(m_objects, [target](const auto& ptr) {
        return ptr.get() == target;
    });
}

void Scene::removeCascade(GeoObject* target) {
    std::vector<GeoObject*> toDelete;
    std::function<void(GeoObject*)> collect = [&](GeoObject* obj) {
      for (GeoObject* dep : obj->dependents()) {
          toDelete.push_back(dep);
          collect(dep);
      }
    };
    collect(target);
    toDelete.push_back(target);

    for (GeoObject* obj : toDelete) obj->detach();
    std::erase_if(m_objects, [&](const auto& ptr) {
        return std::ranges::contains(toDelete, ptr.get());
    });
}

void Scene::softRemove(GeoObject *target) {
    auto it = std::find_if(m_objects.begin(), m_objects.end(), [target](const auto& p) { return p.get() == target; });
    if (it != m_objects.end()) {
        (*it)->detach();
        m_graveyard.push_back(std::move(*it));
        m_objects.erase(it);
    }
}

bool Scene::contains(GeoObject *obj) const {
    return std::ranges::any_of(m_objects, [obj](const auto& ptr) { return ptr.get() == obj; });
}

void Scene::clear() {
    for (auto& obj : m_objects)
        obj->detach();
    m_objects.clear();
}