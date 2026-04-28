#ifndef STOICHEIA_MIDPOINT_H
#define STOICHEIA_MIDPOINT_H
#include "geometry/Line.h"
#include "geometry/Point.h"


class Midpoint : public Point {
    Point* m_a;
    Point* m_b;

public:
    Midpoint(Point *a, Point *b);

    explicit Midpoint(const Line* line);

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;
};


#endif //STOICHEIA_MIDPOINT_H
