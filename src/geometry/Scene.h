#pragma once

#include <memory>
#include <vector>

#include "GeoObject.h"


class Scene {
public:
    // Getter
    [[nodiscard]] auto objects() const -> const std::vector<std::unique_ptr<GeoObject>>& { return m_objects; }
    [[nodiscard]] auto size() const -> size_t { return m_objects.size(); };

    // Erzeugung beliebiger GeoObject-Instanzen
    template<typename T, typename... Args>
    auto create(Args&&... args) -> T* {
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
    auto contains(GeoObject* obj) const -> bool;

    // Aufräumen
    void clear();
    void clearGraveyard() { m_graveyard.clear(); }


private:
    std::vector<std::unique_ptr<GeoObject>> m_objects;
    std::vector<std::unique_ptr<GeoObject>> m_graveyard;
};
