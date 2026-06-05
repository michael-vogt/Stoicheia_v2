#include "Point.h"

#include <format>

Point::Point(double x, double y)
: m_x(x), m_y(y)
{}

void Point::moveTo(const double nx, const double ny) {
    m_x = nx;
    m_y = ny;
    notify();
}

bool Point::equals(const GeoObject &other) const {
    auto p = dynamic_cast<const Point*>(&other);
    if (!p)
        return false;

    return m_x == p->m_x && m_y == p->m_y;
}

std::string Point::toString() {
    std::string str = std::format("({}, {})", m_x, m_y);
    return str;
}


