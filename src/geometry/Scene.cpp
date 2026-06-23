#include "Scene.h"

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <utility>

void Scene::remove(GeoObject* target) {
    target->detach();
    std::erase_if(m_objects, [target](const auto& ptr) -> auto {
        return ptr.get() == target;
    });
}

void Scene::removeCascade(GeoObject* target) {
    std::vector<GeoObject*> to_delete;
    std::function<void(GeoObject*)> collect = [&](GeoObject* obj) -> void {
      for (GeoObject* dep : obj->dependents()) {
          to_delete.push_back(dep);
          collect(dep);
      }
    };
    collect(target);
    to_delete.push_back(target);

    for (GeoObject* obj : to_delete) {
        obj->detach();
    }
    std::erase_if(m_objects, [&](const auto& ptr) -> auto {
        return std::ranges::contains(to_delete, ptr.get());
    });
}

void Scene::softRemove(GeoObject *target) {
    auto iter = std::ranges::find_if(m_objects, [target](const auto& pointer) -> auto { return pointer.get() == target; });
    if (iter != m_objects.end()) {
        (*iter)->detach();
        m_graveyard.push_back(std::move(*iter));
        m_objects.erase(iter);
    }
}

auto Scene::contains(GeoObject *obj) const -> bool {
    return std::ranges::any_of(m_objects, [obj](const auto& ptr) -> auto { return ptr.get() == obj; });
}

void Scene::clear() {
    for (auto& obj : m_objects) {
        obj->detach();
    }
    m_objects.clear();
}

void Scene::restoreFromGraveyard(GeoObject* target) {
    auto iter = std::ranges::find_if(m_graveyard, [target](const auto& ptr) -> auto { return ptr.get() == target; });
    if (iter != m_graveyard.end()) {
        m_objects.push_back(std::move(*iter));
        m_graveyard.erase(iter);
    }
}

void Scene::restoreFromGraveyardWithSources(GeoObject* target, std::initializer_list<GeoObject*> sources) {
    restoreFromGraveyard(target);
    for (GeoObject* src : sources) {
        if (src != nullptr) {
            src->addDependent(target);
        }
    }
    target->recompute();
}