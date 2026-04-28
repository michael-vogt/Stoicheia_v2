#include "PerpendicularFoot.h"

#include <stdexcept>

PerpendicularFoot::PerpendicularFoot(Point *point, Line *line) : Point(0, 0), m_point(point), m_line(line) {
    if (m_point == nullptr || m_line == nullptr) throw std::invalid_argument("null argument");
    m_point->addDependent(this);
    m_line->addDependent(this);
    PerpendicularFoot::recompute();
}

void PerpendicularFoot::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_point)) m_point = nullptr;
    if (src == static_cast<GeoObject*>(m_line)) m_line = nullptr;
}

void PerpendicularFoot::recompute() {
    if (m_point == nullptr || m_line == nullptr) {
        m_valid = false;
        return;
    }

    Point* p1 = m_line->p1();
    Point* p2 = m_line->p2();
    double dx = p2->x() - p1->x();
    double dy = p2->y() - p1->y();
    double len2 = dx*dx + dy*dy;

    if (len2 < 1e-20) { // degenerate line
        m_valid = false;
        return;
    }

    double px = m_point->x() - p1->x();
    double py = m_point->y() - p1->y();
    double t = (px * dx + py * dy) / len2;

    m_valid = true;
    moveTo(p1->x() + t * dx, p1->y() + t * dy);
}

std::string PerpendicularFoot::toString() {
    return Point::toString();
}
