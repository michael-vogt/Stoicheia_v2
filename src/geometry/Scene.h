#pragma once

#include <memory>
#include <vector>

#include "GeoObject.h"


class Scene {
public:
    // Getter
    [[nodiscard]] const std::vector<std::unique_ptr<GeoObject>>& objects() const { return m_objects; }
    [[nodiscard]] size_t size() const { return m_objects.size(); };

    // Erzeugung beliebiger GeoObject-Instanzen
    template<typename T, typename... Args>
    T* create(Args&&... args) {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = obj.get();
        m_objects.push_back(std::move(obj));
        return ptr;
    }

    // Objekte entfernen
    void remove(GeoObject* target);
    void removeCascade(GeoObject* target);
    void softRemove(GeoObject* target);

    // Existenz-Check
    bool contains(GeoObject* obj) const;

    // Aufräumen
    void clear();
    void clearGraveyard() { m_graveyard.clear(); }


private:
    std::vector<std::unique_ptr<GeoObject>> m_objects;
    std::vector<std::unique_ptr<GeoObject>> m_graveyard;
};
