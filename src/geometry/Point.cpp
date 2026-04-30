#include "Point.h"

#include <format>

void Point::moveTo(const double nx, const double ny) {
    m_x = nx;
    m_y = ny;
    notify();
}

std::string Point::toString() {
    std::string str = std::format("({}, {})", m_x, m_y);
    return str;
}


