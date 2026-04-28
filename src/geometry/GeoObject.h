#ifndef STOICHEIA_GEOOBJECT_H
#define STOICHEIA_GEOOBJECT_H
#include <string>
#include <unordered_set>


class GeoObject {
    void enqueueTransitive() const;
protected:
    std::unordered_set<GeoObject*> m_dependents;
    std::unordered_set<GeoObject*> m_sources;
    bool m_valid = true;
    void notifyDependents(std::unordered_set<GeoObject*>& visited) const;
    void notify();
public:
    virtual ~GeoObject() = default;
    void addDependent(GeoObject* dep);
    std::unordered_set<GeoObject*> dependents();
    void detach();
    bool isValid() const;
    virtual void onSourceRemoved(GeoObject* src);
    void removeDependent(GeoObject* dep);
    virtual void recompute() = 0;
    virtual std::string toString() = 0;
};


#endif //STOICHEIA_GEOOBJECT_H
