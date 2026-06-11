#include "Perpendicular.h"
#include "Structs.h"

#include <stdexcept>

Perpendicular::Perpendicular(Point* origin, LinearObject* reference)
: m_origin(origin), m_reference(reference),
m_phantom(origin->x() + px(), origin->y() + py())
{
    if (m_origin == nullptr || m_reference == nullptr) { 
        throw std::invalid_argument("null argument");
    }
    PointPairForLinearObject points{.point1=&m_phantom, .point2=origin};
    m_line = new Line(points);
    m_origin->addDependent(this);
    m_reference->addDependent(this);
    Perpendicular::recompute();
}

void Perpendicular::onSourceRemoved(GeoObject* src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_origin)) { m_origin = nullptr; }
    if (src == static_cast<GeoObject*>(m_reference)) { m_reference = nullptr; }
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

void Perpendicular::replaceSource(GeoObjectPair source) {
    if (m_origin == source.oldGeoObject) { 
        m_origin = static_cast<Point*>(source.newGeoObject); 
    }
    if (m_reference == source.oldGeoObject) { 
        m_reference = static_cast<LinearObject*>(source.newGeoObject);
    }
}

auto Perpendicular::px() const -> double {
    return -(m_reference->point2()->y() - m_reference->point1()->y());
}

auto Perpendicular::py() const -> double {
    return  (m_reference->point2()->x() - m_reference->point1()->x());
}