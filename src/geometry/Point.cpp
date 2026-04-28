#include "Point.h"

#include <format>

/*
 * Constructor
 */
Point::Point(double x, double y) : m_x(x), m_y(y) {
}

/*
 * Getter
 */
double Point::x() const {
    return m_x;
}

double Point::y() const {
    return m_y;
}

/*
 * Modifier
 */
void Point::moveTo(const double nx, const double ny) {
    m_x = nx;
    m_y = ny;
    notify();
}

/*
 * Overridden methods
 */
void Point::recompute() {
    // nothing to do for a point
}

std::string Point::toString() {
    std::string str = std::format("({}, {})", m_x, m_y);
    return str;
}


