#pragma once

#include <string>
#include <unordered_set>
#include <typeinfo>

class UpdateGuard;
void updateGuardDequeue(class GeoObject* obj);

class GeoObject {
public:
    virtual ~GeoObject() { detach(); }

    // Getter
    bool isValid() const { return m_valid; }
    std::unordered_set<GeoObject*> dependents() { return m_dependents; }
    std::unordered_set<GeoObject*>& sources() { return m_sources; }

    // Objekt aktualisieren
    void addDependent(GeoObject* dep);
    void detach();
    virtual void onSourceRemoved(GeoObject* src) { m_valid = false; };
    virtual void recompute() = 0;
    void removeDependent(GeoObject* dep);
    virtual void replaceSource(GeoObject* oldSource, GeoObject* newSource) {};

    virtual bool equals(const GeoObject& other) const = 0;
    virtual std::string toString() { return ""; };

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

inline bool operator==(const GeoObject& obj1, const GeoObject& obj2) {
    return obj1.equals(obj2);
}