#pragma once

#include "LinearObject.h"

class Line : public LinearObject {
public:
    using LinearObject::LinearObject;

protected:
    [[nodiscard]] auto containsT(double param_t) const -> bool override { return true; };

};