#include "Scene.h"

void Scene::remove(GeoObject* target) {
    if (target == nullptr) return;

    target->detach();
    std::erase_if(m_objects, [target](const auto& ptr) {
        return ptr.get() == target;
    });
}

size_t Scene::size() const {
    return m_objects.size();
}
