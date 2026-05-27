#pragma once

#include <memory>
#include <vector>

#include "GeoObject.h"


class Scene {
    std::vector<std::unique_ptr<GeoObject>> m_objects;
    //Scene() = default;
public:
    /*static Scene& getInstance() {
        static Scene instance;
        return instance;
    }*/

    template<typename T, typename... Args>
    T* create(Args&&... args) {
        T temp(std::forward<Args>(args)...);
        for (auto& object : m_objects) {
            if (auto casted = dynamic_cast<T*>(object.get())) {
                if (*casted == temp)
                    return casted;
            }
        }

        auto obj = std::make_unique<T>(std::move(temp)); //(std::forward<Args>(args)...);
        T* ptr = obj.get();
        m_objects.push_back(std::move(obj));
        return ptr;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<GeoObject>>& objects() const { return m_objects; }
    
    void remove(GeoObject* target);
    void removeCascade(GeoObject* target);
    bool contains(GeoObject* obj) const;

    void clear();

    [[nodiscard]] size_t size() const { return m_objects.size(); };
};
