#pragma once

#include "LinearObject.h"


class Ray : public LinearObject {
protected:
    bool containsT(double t) const override { return t >= -1e-10; };
public:
    using LinearObject::LinearObject;
    Point* origin() const { return m_p1; };
    Point* direction() const { return m_p2; };
};