#pragma once

#include <unordered_set>
#include "../Structs.h"

class UpdateGuard;
void updateGuardDequeue(class GeoObject* obj);

class GeoObject {
public:
    virtual ~GeoObject() { detach(); }

    // Getter
    [[nodiscard]] auto isValid() const -> bool { return m_valid; }
    auto dependents() -> std::unordered_set<GeoObject*> { return m_dependents; }
    auto sources() -> std::unordered_set<GeoObject*>& { return m_sources; }

    // Objekt aktualisieren
    void addDependent(GeoObject* dep);
    void detach();
    virtual void onSourceRemoved(GeoObject* src) { m_valid = false; };
    virtual void recompute() = 0;
    void removeDependent(GeoObject* dep);
    virtual void replaceSource(GeoObjectPair source) {};

protected:
    // notify and helpers are implemented in UpdateGuard.cpp
    void enqueueTransitive() const;
    void notify();
    void notifyDirect();

    std::unordered_set<GeoObject*> m_dependents;
    std::unordered_set<GeoObject*> m_sources;
    bool m_valid = true;

private:
    friend class UpdateGuard;
};