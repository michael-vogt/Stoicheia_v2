#include "Parallel.h"
#include "Structs.h"

#include <stdexcept>

Parallel::Parallel(Point *origin, LinearObject *reference)
: m_origin(origin), m_phantom(origin->x(), origin->y()), m_reference(reference)
{
    if (m_origin == nullptr || m_reference == nullptr) {
        throw std::invalid_argument("null argument");
    }
    PointPairForLinearObject pointPair{.point1=&m_phantom, .point2=origin};
    m_line = new Line(pointPair);
    m_origin->addDependent(this);
    m_reference->addDependent(this);
    Parallel::recompute();
}

void Parallel::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_origin)) { m_origin = nullptr; }
    if (src == static_cast<GeoObject*>(m_reference)) { m_reference = nullptr; }
}

void Parallel::recompute() {
    if (m_origin == nullptr || m_reference == nullptr) {
        m_valid = false;
        return;
    }

    m_valid = true;

    double dir_x = dx();
    double dir_y = dy();
    m_phantom.moveTo(m_origin->x() + dir_x, m_origin->y() + dir_y);
    notify();
}

void Parallel::replaceSource(GeoObjectPair source) {
    if (m_origin == source.oldGeoObject) { m_origin = static_cast<Point*>(source.newGeoObject); }
    if (m_reference == source.oldGeoObject) { m_reference = static_cast<LinearObject*>(source.newGeoObject); }
}

auto Parallel::dx() const -> double {
    auto [p1, p2] = m_reference->points();
    return p2->x() - p1->x();
}

auto Parallel::dy() const -> double {
    auto [p1, p2] = m_reference->points();
    return p2->y() - p1->y();
}