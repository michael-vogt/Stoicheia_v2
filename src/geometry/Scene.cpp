#include "Scene.h"

#include <algorithm>
#include <functional>

template<typename T>
std::vector<T *> Scene::objectsOfType() const {
    std::vector<T*> result;

    for (const auto& obj : m_objects) {
        if (auto* typed = dynamic_cast<T*>(obj.get()))
            result.push_back(typed);
    }
    return result;
}

void Scene::remove(GeoObject* target) {
    if (target == nullptr) return;

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
