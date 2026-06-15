#include "ConstrainedPoint.h"
#include <cmath>

// ─── ConstrainedPointOnLine ───────────────────────────────────────────────────

ConstrainedPointOnLine::ConstrainedPointOnLine(LinearObject* line, const DoublePair point)
    : Point(point.x, point.y), m_line(line)
{
    m_param = m_line->projectParameter({point.x, point.y});
    //m_line->addDependent(this);
    m_line->point1()->addDependent(this);
    m_line->point2()->addDependent(this);
    ConstrainedPointOnLine::recompute();
}

void ConstrainedPointOnLine::setParam(const double t) {
    m_param = t;
    recompute();
}

void ConstrainedPointOnLine::recompute() {
    if (m_line == nullptr || !m_line->isValid()) {
        m_valid = false;
        return;
    }
    const double x = m_line->point1()->x() + m_param * m_line->dx();
    const double y = m_line->point1()->y() + m_param * m_line->dy();
    moveTo(x, y);
    m_valid = true;
}

void ConstrainedPointOnLine::onSourceRemoved(GeoObject* src) {
    if (src == m_line) m_line = nullptr;
    m_valid = false;
}

void ConstrainedPointOnLine::replaceSource(const GeoObjectPair source) {
    if (m_line == source.oldGeoObject) {
        m_line = static_cast<LinearObject*>(source.newGeoObject);
    }
}

// ─── ConstrainedPointOnCircle ────────────────────────────────────────────────

ConstrainedPointOnCircle::ConstrainedPointOnCircle(Circle* circle, const DoublePair point)
    : Point(point.x, point.y), m_circle(circle)
{
    const double dx = point.x - circle->center()->x();
    const double dy = point.y - circle->center()->y();
    m_angle = std::atan2(dy, dx);

    //m_circle->addDependent(this);
    m_circle->center()->addDependent(this);
    m_circle->radiusPoint()->addDependent(this);

    ConstrainedPointOnCircle::recompute();
}

void ConstrainedPointOnCircle::setAngle(const double angle) {
    m_angle = angle;
    recompute();
}

void ConstrainedPointOnCircle::recompute() {
    if (!m_circle || !m_circle->isValid()) {
        m_valid = false;
        return;
    }
    const double x = m_circle->center()->x() + m_circle->radius() * std::cos(m_angle);
    const double y = m_circle->center()->y() + m_circle->radius() * std::sin(m_angle);
    moveTo(x, y);
    m_valid = true;
}

void ConstrainedPointOnCircle::onSourceRemoved(GeoObject* src) {
    if (src == m_circle) m_circle = nullptr;
    m_valid = false;
}

void ConstrainedPointOnCircle::replaceSource(const GeoObjectPair source) {
    if (m_circle == source.oldGeoObject) {
        m_circle = static_cast<Circle*>(source.newGeoObject);
    }
}