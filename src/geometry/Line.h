#pragma once

#include "LinearObject.h"

class Line : public LinearObject {
protected:
    bool containsT(double) const override { return true; };
public:
    using LinearObject::LinearObject;

};