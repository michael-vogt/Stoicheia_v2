#include "Midpoint.h"

#include <stdexcept>

Midpoint::Midpoint(Point *a, Point *b) : Point(0, 0), m_a(a), m_b(b) {
    if (m_a == nullptr || m_b == nullptr)
        throw std::invalid_argument("null point");
    m_a->addDependent(this);
    m_b->addDependent(this);
    Midpoint::recompute();
}

Midpoint::Midpoint(const Line *line) : Midpoint(line->p1(), line->p2()) {}

void Midpoint::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_a)) m_a = nullptr;
    if (src == static_cast<GeoObject*>(m_b)) m_b = nullptr;
}

void Midpoint::recompute() {
    if (m_a == nullptr || m_b == nullptr) {
        m_valid = false;
        return;
    }

    m_valid = true;
    double x = (m_a->x() + m_b->x()) / 2.0;
    double y = (m_a->y() + m_b->y()) / 2.0;
    moveTo(x, y);
}

std::string Midpoint::toString() {
    return Point::toString();
}
