#include "Perpendicular.h"

#include <stdexcept>

Perpendicular::Perpendicular(Point* origin, LinearObject* reference)
        : m_origin(origin), m_reference(reference),
          m_phantom(origin->x() + px(), origin->y() + py()),
          m_line(&m_phantom, origin)
{
    if (m_origin == nullptr || m_reference == nullptr) throw std::invalid_argument("null argument");
    m_origin->addDependent(this);
    m_reference->addDependent(this);
    Perpendicular::recompute();
}

void Perpendicular::recompute() {
    if (m_origin == nullptr || m_reference == nullptr) {
        m_valid = false;
        return;
    }

    m_valid = true;
    m_phantom.moveTo(m_origin->x() + px(), m_origin->y() + py());
    notify();
}

void Perpendicular::onSourceRemoved(GeoObject* src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_origin)) m_origin = nullptr;
    if (src == static_cast<GeoObject*>(m_reference)) m_reference = nullptr;
}

double Perpendicular::px() const {
    return -(m_reference->p2()->y() - m_reference->p1()->y());
}

double Perpendicular::py() const {
    return  (m_reference->p2()->x() - m_reference->p1()->x());
}

std::string Perpendicular::toString() {
    return m_line.toString();
}
