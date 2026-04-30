#pragma once

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

    [[nodiscard]] size_t size() const { return m_objects.size(); };
};
