#pragma once

#include "LinearObject.h"


class Ray : public LinearObject {
protected:
    bool containsT(double t) const override { return t >= -1e-10; };
public:
    using LinearObject::LinearObject;
    Point* origin() const { return m_p1; };
    Point* direction() const { return m_p2; };

    bool equals(const GeoObject &other) const override {
        auto r = dynamic_cast<const Ray*>(&other);
        if (!r)
            return false;
        return r->p1()->equals(*p1()) && r->p2()->equals(*p2());
    }
};