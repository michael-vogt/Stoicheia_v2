#pragma once

#include "LinearObject.h"


class Ray : public LinearObject {
public:
    using LinearObject::LinearObject;

    // Getter
    Point* origin() const { return m_p1; };
    Point* direction() const { return m_p2; };

    bool equals(const GeoObject &other) const override {
        auto r = dynamic_cast<const Ray*>(&other);
        if (!r)
            return false;
        return r->p1()->equals(*p1()) && r->p2()->equals(*p2());
    }

protected:
    bool containsT(const double t) const override { return t >= -1e-10; };
};