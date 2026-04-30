#include "LineCircleIntersection.h"

#include <complex>

LineCircleIntersection::LineCircleIntersection(LinearObject *line, Circle *circle) : m_line(line), m_circle(circle) {
    if (m_line == nullptr || m_circle == nullptr)
        throw std::invalid_argument("null argument");
    m_line->addDependent(this);
    m_circle->addDependent(this);
    IntersectionSet::recompute();
}

void LineCircleIntersection::compute() {
    if (m_line == nullptr || m_circle == nullptr) {
        setResults(0);
        return;
    }

    const double mx = m_circle->center()->x();
    const double my = m_circle->center()->y();
    const double r = m_circle->radius();

    const double x1 = m_line->p1()->x();
    const double y1 = m_line->p1()->y();
    const double x2 = m_line->p2()->x();
    const double y2 = m_line->p2()->y();

    const double dx = x2 - x1;
    const double dy = y2 - y1;

    const double a = dx*dx + dy*dy;
    if (a < 1e-20) { // degenerated line
        setResults(0);
        return;
    }

    const double b = 2.0 * (dx * (x1 - mx) + dy * (y1 - my));
    const double c = (x1 - mx) * (x1 - mx) + (y1 - my) * (y1 - my) - r * r;

    if (const double disc = b*b - 4*a*c; disc < -1e-10) { // no intersection
        setResults(0);
    } else if (disc < 1e-10) {
        const double t = -b / (2*a);
        setResults(1, x1 + t*dx, y1 + t*dy);
    } else {
        const double sqrtDisc = std::sqrt(disc);
        const double t1 = (-b - sqrtDisc) / (2*a);
        const double t2 = (-b + sqrtDisc) / (2*a);
        setResults(2, x1 + t1*dx, y1 + t1*dy,x1 + t2*dx, y1 + t2*dy);
    }
}

void LineCircleIntersection::onSourceRemoved(GeoObject *src) {
    if (src == static_cast<GeoObject*>(m_line)) m_line = nullptr;
    if (src == static_cast<GeoObject*>(m_circle)) m_circle = nullptr;
    IntersectionSet::onSourceRemoved(src);
}
