#pragma once

#include "LinearObject.h"

class Line : public LinearObject {
protected:
    bool containsT(double) const override { return true; };
public:
    using LinearObject::LinearObject;

    bool equals(const GeoObject &other) const override {
        auto l = dynamic_cast<const Line*>(&other);
        if (!l)
            return false;
        return l->p1()->equals(*p1()) && l->p2()->equals(*p2());
    }

};