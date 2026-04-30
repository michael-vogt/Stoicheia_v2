#ifndef STOICHEIA_INTERSECTIONSET_H
#define STOICHEIA_INTERSECTIONSET_H
#include "IntersectionPoint.h"
#include "geometry/GeoObject.h"


class IntersectionSet : public GeoObject {
    IntersectionPoint m_pts[2];
protected:
    void setResults(int count, double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0);
    virtual void compute() = 0;
public:
    IntersectionSet();
    IntersectionPoint* first() { return &m_pts[0]; };
    IntersectionPoint* second() { return &m_pts[1]; };

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;
};


#endif //STOICHEIA_INTERSECTIONSET_H
