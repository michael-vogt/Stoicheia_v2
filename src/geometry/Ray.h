#pragma once

#include "LinearObject.h"
#include <limits>


class Ray : public LinearObject {
public:
    using LinearObject::LinearObject;

    // Getter
    [[nodiscard]] auto origin() const -> Point* { return m_point1; };
    [[nodiscard]] auto direction() const -> Point* { return m_point2; };

protected:
    [[nodiscard]] auto containsT(const double param_t) const -> bool override { return param_t >= -std::numeric_limits<double>::epsilon() ; };
};