#pragma once

#include "LinearObject.h"


class Ray : public LinearObject {
public:
    using LinearObject::LinearObject;

    // Getter
    Point* origin() const { return m_p1; };
    Point* direction() const { return m_p2; };

protected:
    bool containsT(const double t) const override { return t >= -1e-10; };
};