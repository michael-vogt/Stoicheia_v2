#pragma once

#include "LinearObject.h"

class Line : public LinearObject {
public:
    using LinearObject::LinearObject;

protected:
    bool containsT(double) const override { return true; };

};