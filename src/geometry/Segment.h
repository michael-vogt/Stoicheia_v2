#pragma once

#include "LinearObject.h"
#include <limits>

constexpr double eps = std::numeric_limits<double>::epsilon();

class Segment : public LinearObject {
public:
    using LinearObject::LinearObject;
    [[nodiscard]] auto start() const -> Point* { return m_point1; };
    [[nodiscard]] auto end() const -> Point* { return m_point2; };

protected:
    [[nodiscard]] auto containsT(double param_t) const -> bool override { return -eps <= param_t && param_t <= 1.0 + eps; };
};