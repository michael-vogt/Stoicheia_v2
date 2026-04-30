#include "Parallel.h"

#include <stdexcept>

Parallel::Parallel(Point *origin, LinearObject *reference)
: m_origin(origin), m_reference(reference), m_phantom(origin->x(), origin->y()), m_line(&m_phantom, origin)
{
    if (m_origin == nullptr || m_reference == nullptr)
        throw std::invalid_argument("null argument");
    m_origin->addDependent(this);
    m_reference->addDependent(this);
    Parallel::recompute();
}

double Parallel::dx() const {
    auto [p1, p2] = m_reference->points();
    return p2->x() - p1->x();
}

double Parallel::dy() const {
    auto [p1, p2] = m_reference->points();
    return p2->y() - p1->y();
}

void Parallel::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_origin)) m_origin = nullptr;
    if (src == static_cast<GeoObject*>(m_reference)) m_reference = nullptr;
}

void Parallel::recompute() {
    if (m_origin == nullptr || m_reference == nullptr) {
        m_valid = false;
        return;
    }

    m_valid = true;

    double vx = dx(), vy = dy();
    m_phantom.moveTo(m_origin->x() + vx, m_origin->y() + vy);
    notify();
}

std::string Parallel::toString() {
    return m_line.toString();
}
