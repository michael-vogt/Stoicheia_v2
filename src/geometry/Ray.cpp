#include "Ray.h"

bool Ray::containsT(double t) const {
    return t >= -1e-10;
}

Point* Ray::direction() const {
    return p2();
}

Point* Ray::origin() const {
    return p1();
}
