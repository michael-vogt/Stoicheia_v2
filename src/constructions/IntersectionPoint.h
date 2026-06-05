#pragma once

#include "../geometry/geometry.h"

class IntersectionSet;

class IntersectionPoint : public Point {
public:
    IntersectionPoint() : Point(0,0) {}
    void recompute() override {}

private:
    friend class IntersectionSet;
    using Point::moveTo;
};