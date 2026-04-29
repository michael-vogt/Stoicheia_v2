#ifndef STOICHEIA_SCENE_H
#define STOICHEIA_SCENE_H
#include <memory>
#include <vector>

#include "GeoObject.h"


class Scene {
    std::vector<std::unique_ptr<GeoObject>> m_objects;
public:
    template<typename T, typename... Args>
    T* create(Args&&... args) {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = obj.get();
        m_objects.push_back(std::move(obj));
        return ptr;
    }

    void remove(GeoObject* target);
    void removeCascade(GeoObject* target);

    size_t size() const;
};


#endif //STOICHEIA_SCENE_H
