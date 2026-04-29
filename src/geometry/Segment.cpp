#include "Segment.h"

bool Segment::containsT(double t) const {
    return -1e-10 <= t && t <= 1.0 + 1e-10;
}

Point* Segment::end() const {
    return p2();
}

Point* Segment::start() const {
    return p1();
}
