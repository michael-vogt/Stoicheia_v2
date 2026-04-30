#pragma once

#include <memory>
#include <vector>

#include "GeoObject.h"


class Scene {
    std::vector<std::unique_ptr<GeoObject>> m_objects;
    Scene() = default;
public:
    static Scene& getInstance() {
        static Scene instance;
        return instance;
    }

    template<typename T, typename... Args>
    T* create(Args&&... args) {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = obj.get();
        m_objects.push_back(std::move(obj));
        return ptr;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<GeoObject>>& objects() const { return m_objects; }
    template<typename T>
    std::vector<T*> objectsOfType() const {
        std::vector<T*> result;

        for (const auto& obj : m_objects) {
            if (auto* typed = dynamic_cast<T*>(obj.get()))
                result.push_back(typed);
        }
        return result;
    };
    void remove(GeoObject* target);
    void removeCascade(GeoObject* target);

    [[nodiscard]] size_t size() const { return m_objects.size(); };
};
