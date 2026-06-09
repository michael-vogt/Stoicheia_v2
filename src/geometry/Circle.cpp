#include "Circle.h"
#include "Structs.h"
#include <stdexcept>
#include <cmath>


Circle::Circle(PointPairForCircle points)
: m_center(points.center), m_radiusPoint(points.radiusPoint)
{
    if (m_center == nullptr || m_radiusPoint == nullptr) {
        throw std::invalid_argument("null point");
    }
    m_center->addDependent(this);
    m_radiusPoint->addDependent(this);
    Circle::recompute();
}

Circle::Circle(Point *center, double radius)
: m_fixedRadius(radius),
  m_fixedRadiusPoint(std::make_unique<Point>(center->x() + radius, center->y())), m_center(center)
{
    if (m_center == nullptr) {
        throw std::invalid_argument("null point");
    }
    m_radiusPoint = m_fixedRadiusPoint.get();
    m_center->addDependent(this);
    Circle::recompute();
}

void Circle::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_center)) { 
        m_center = nullptr;
    }
    if (src == static_cast<GeoObject*>(m_radiusPoint)) {
        m_radiusPoint = nullptr;
    }
}

void Circle::recompute() {
    if (m_center == nullptr || m_radiusPoint == nullptr) {
        m_valid = false;
        return;
    }

    if (m_fixedRadiusPoint) {
        m_fixedRadiusPoint->moveTo(m_center->x() + m_fixedRadius, m_center->y());
    }

    double delta_x = m_radiusPoint->x() - m_center->x();
    double delta_y = m_radiusPoint->y() - m_center->y();
    m_radius = sqrt((delta_x*delta_x) + (delta_y*delta_y));
    m_valid = true;
    notify();
}

void Circle::replaceSource(GeoObjectPair source) {
    if (m_center == source.oldGeoObject) { 
        m_center = static_cast<Point*>(source.newGeoObject);
    }
    if (m_radiusPoint == source.oldGeoObject) {
        m_radiusPoint = static_cast<Point*>(source.newGeoObject);
    }
}