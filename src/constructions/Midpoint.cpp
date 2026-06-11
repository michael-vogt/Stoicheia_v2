#include "Midpoint.h"
#include "Structs.h"

#include <stdexcept>

Midpoint::Midpoint(Point *point1, Point *point2)
: Point(0, 0), m_point1(point1), m_point2(point2)
{
    if (m_point1 == nullptr || m_point2 == nullptr) {
        throw std::invalid_argument("null point");
    }
    m_point1->addDependent(this);
    m_point2->addDependent(this);
    Midpoint::recompute();
}

Midpoint::Midpoint(const LinearObject *line)
: Midpoint(line->point1(), line->point2())
{
}

void Midpoint::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_point1)) { m_point1 = nullptr; }
    if (src == static_cast<GeoObject*>(m_point2)) { m_point2 = nullptr; }
}

void Midpoint::recompute() {
    if (m_point1 == nullptr || m_point2 == nullptr) {
        m_valid = false;
        return;
    }

    m_valid = true;
    double coord_x = (m_point1->x() + m_point2->x()) / 2;
    double coord_y = (m_point1->y() + m_point2->y()) / 2;
    moveTo(coord_x, coord_y);
}

void Midpoint::replaceSource(GeoObjectPair source) {
    if (m_point1 == source.oldGeoObject) { m_point1 = static_cast<Point*>(source.newGeoObject); }
    if (m_point2 == source.oldGeoObject) { m_point2 = static_cast<Point*>(source.newGeoObject); }
}