#pragma once

#include "LinearObject.h"


class Segment : public LinearObject {
protected:
    bool containsT(double t) const override { return -1e-10 <= t && t <= 1.0 + 1e-10; };
public:
    using LinearObject::LinearObject;
    Point* start() const { return m_p1; };
    Point* end() const { return m_p2; };
};