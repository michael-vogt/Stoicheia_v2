#ifndef STOICHEIA_PARALLEL_H
#define STOICHEIA_PARALLEL_H
#include "geometry/GeoObject.h"
#include "geometry/Line.h"
#include "geometry/LinearObject.h"
#include "geometry/Point.h"


class Parallel : public GeoObject {
    Point* m_origin;
    Point m_phantom;
    LinearObject* m_reference;
    Line m_line;

    double dx() const;
    double dy() const;

public:
    Parallel(Point* origin, LinearObject* reference);
    LinearObject* line();
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;

};


#endif //STOICHEIA_PARALLEL_H
