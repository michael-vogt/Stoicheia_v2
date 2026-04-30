#ifndef STOICHEIA_INTERSECTION_H
#define STOICHEIA_INTERSECTION_H

#include "../geometry/geometry.h"

class IntersectionSet;

class IntersectionPoint : public Point {
    friend class IntersectionSet;
    using Point::moveTo;

public:
    IntersectionPoint() : Point(0,0) {};
    void recompute() override {};
};


#endif //STOICHEIA_INTERSECTION_H
