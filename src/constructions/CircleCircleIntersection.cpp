#include "CircleCircleIntersection.h"

CircleCircleIntersection::CircleCircleIntersection(Circle *c1, Circle *c2) : m_c1(c1), m_c2(c2)
{
    if (!c1 || !c2) throw std::invalid_argument("null argument");
    c1->addDependent(this);
    c2->addDependent(this);
    IntersectionSet::recompute();
}

void CircleCircleIntersection::compute()  {
    if (!m_c1 || !m_c2) { setResults(0); return; }

    double x1 = m_c1->center()->x(), y1 = m_c1->center()->y();
    double x2 = m_c2->center()->x(), y2 = m_c2->center()->y();
    double r1 = m_c1->radius(), r2 = m_c2->radius();

    double dx = x2 - x1, dy = y2 - y1;
    double d = std::sqrt(dx*dx + dy*dy);

    // Keine Schnittpunkte: zu weit auseinander oder einer im anderen
    if (d > r1 + r2 + 1e-10 || d < std::abs(r1 - r2) - 1e-10) {
        setResults(0); return;
    }
    // Identische Kreise
    if (d < 1e-10 && std::abs(r1 - r2) < 1e-10) {
        setResults(0); return; // unendlich viele – nicht darstellbar
    }

    double a = (d*d + r1*r1 - r2*r2) / (2*d);
    double h2 = r1*r1 - a*a;

    // Fußpunkt auf Verbindungslinie
    double px = x1 + a * dx/d;
    double py = y1 + a * dy/d;

    if (h2 < 1e-10) {
        // Tangente: ein Schnittpunkt
        setResults(1, px, py);
    } else {
        double h = std::sqrt(h2);
        // Normalenvektor: senkrecht zu (dx,dy)
        double nx = -dy/d, ny = dx/d;
        setResults(2,
            px + h*nx, py + h*ny,
            px - h*nx, py - h*ny);
    }
}

void CircleCircleIntersection::onSourceRemoved(GeoObject *src)  {
    if (src == static_cast<GeoObject*>(m_c1)) m_c1 = nullptr;
    if (src == static_cast<GeoObject*>(m_c2)) m_c2 = nullptr;
    IntersectionSet::onSourceRemoved(src);
}
