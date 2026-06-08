#include "Point.h"

Point::Point(double pos_x, double pos_y)
: m_x(pos_x), m_y(pos_y)
{}

void Point::moveTo(const double new_x, const double new_y) {
    m_x = new_x;
    m_y = new_y;
    notify();
}