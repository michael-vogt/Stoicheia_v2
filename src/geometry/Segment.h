#pragma once

#include "LinearObject.h"


class Segment : public LinearObject {
public:
    using LinearObject::LinearObject;
    Point* start() const { return m_p1; };
    Point* end() const { return m_p2; };

    bool equals(const GeoObject &other) const override {
        auto s = dynamic_cast<const Line*>(&other);
        if (!s)
            return false;
        return s->p1()->equals(*p1()) && s->p2()->equals(*p2());
    }

protected:
    bool containsT(double t) const override { return -1e-10 <= t && t <= 1.0 + 1e-10; };
};