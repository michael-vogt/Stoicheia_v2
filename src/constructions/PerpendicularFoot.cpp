#include "PerpendicularFoot.h"
#include "Structs.h"

#include <limits>
#include <stdexcept>

constexpr double eps = std::numeric_limits<double>::epsilon();

PerpendicularFoot::PerpendicularFoot(Point *point, LinearObject *line)
: Point(0, 0), m_point(point), m_line(line)
{
    if (m_point == nullptr || m_line == nullptr) {
        throw std::invalid_argument("null argument");
    }
    m_point->addDependent(this);
    m_line->addDependent(this);
    PerpendicularFoot::recompute();
}

void PerpendicularFoot::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_point)) {
        m_point = nullptr;
    }
    if (src == static_cast<GeoObject*>(m_line)) {
        m_line = nullptr;
    }
}

void PerpendicularFoot::recompute() {
    if (m_point == nullptr || m_line == nullptr) {
        m_valid = false;
        return;
    }

    const Point* point1 = m_line->point1();
    const Point* point2 = m_line->point2();
    const double delta_x = point2->x() - point1->x();
    const double delta_y = point2->y() - point1->y();
    const double len2 = (delta_x*delta_x) + (delta_y*delta_y);

    if (len2 < eps*eps) { // degenerate line
        m_valid = false;
        return;
    }

    // Projektion von point1->m_point auf point1->point2
    const double dir_x = m_point->x() - point1->x();
    const double dir_y = m_point->y() - point1->y();
    const double param_t = ((dir_x * delta_x) + (dir_y * delta_y)) / len2;

    m_valid = true;
    moveTo(point1->x() + (param_t * delta_x), point1->y() + (param_t * delta_y));
}

void PerpendicularFoot::replaceSource(GeoObjectPair source) {
    if (m_point == source.oldGeoObject) { 
        m_point = static_cast<Point*>(source.newGeoObject); 
    }
    if (m_line == source.oldGeoObject) { 
        m_line = static_cast<LinearObject*>(source.newGeoObject);
    }
}