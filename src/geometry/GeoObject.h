#ifndef STOICHEIA_GEOOBJECT_H
#define STOICHEIA_GEOOBJECT_H
#include <string>
#include <unordered_set>

class UpdateGuard;

void updateGuardDequeue(class GeoObject* obj);

class GeoObject {
    friend class UpdateGuard;
protected:
    std::unordered_set<GeoObject*> m_dependents;
    std::unordered_set<GeoObject*> m_sources;
    bool m_valid = true;

    //void notifyDependents(std::unordered_set<GeoObject*>& visited) const;
    void notify();
    void notifyDirect();
    void enqueueTransitive() const;
public:
    virtual ~GeoObject() { detach(); };
    void addDependent(GeoObject* dep);
    void removeDependent(GeoObject* dep);
    void detach();
    virtual void onSourceRemoved(GeoObject* src) { m_valid = false; };
    virtual void recompute() = 0;
    bool isValid() const { return m_valid; };
    std::unordered_set<GeoObject*> dependents() { return m_dependents; };

    virtual std::string toString() = 0;
};


#endif //STOICHEIA_GEOOBJECT_H
