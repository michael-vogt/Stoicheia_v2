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