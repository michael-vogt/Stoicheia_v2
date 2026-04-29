#include "Circle.h"

#include <complex>
#include <format>

Circle::Circle(Point *center, Point* radiusPoint) : m_center(center), m_radiusPoint(radiusPoint) {
    if (m_center == nullptr || m_radiusPoint == nullptr)
        throw std::invalid_argument("null point");
    m_center->addDependent(this);
    m_radiusPoint->addDependent(this);
    Circle::recompute();
}

Circle::Circle(Point *center, double radius)
    : m_fixedRadius(radius),
      m_fixedRadiusPoint(std::make_unique<Point>(
          center->x() + radius, center->y())), m_center(center)
{
    if (m_center == nullptr)
        throw std::invalid_argument("null point");
    m_radiusPoint = m_fixedRadiusPoint.get();
    m_center->addDependent(this);
    Circle::recompute();
}

Point* Circle::center() const {
    return m_center;
}

double Circle::radius() const {
    return m_radius;
}

void Circle::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_center)) m_center = nullptr;
    if (src == static_cast<GeoObject*>(m_radiusPoint)) m_radiusPoint = nullptr;
}

void Circle::recompute() {
    if (m_center == nullptr || m_radiusPoint == nullptr) {
        m_valid = false;
        return;
    }

    if (m_fixedRadiusPoint) {
        m_fixedRadiusPoint->moveTo(m_center->x() + m_fixedRadius, m_center->y());
    }

    double dx = m_radiusPoint->x() - m_center->x();
    double dy = m_radiusPoint->y() - m_center->y();
    m_radius = std::sqrt(dx*dx + dy*dy);
    m_valid = true;
    notify();
}

std::string Circle::toString() {
    std::string str = std::format("C[center {}, radius {}]", m_center->toString(), m_radius);
    return str;
}
