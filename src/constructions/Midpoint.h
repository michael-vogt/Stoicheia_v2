#pragma once

#include "geometry/LinearObject.h"
#include "geometry/Point.h"


class Midpoint : public Point {
    Point* m_a;
    Point* m_b;

public:
    Midpoint(Point *a, Point *b);

    explicit Midpoint(const LinearObject* line) : Midpoint(line->p1(), line->p2()) {}

    Point* p1() const { return m_a; }
    Point* p2() const { return m_b; }

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;
};