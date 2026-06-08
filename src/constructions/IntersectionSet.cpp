#include "IntersectionSet.h"
#include "Structs.h"
#include <stdexcept>

void IntersectionSet::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    m_pts[0].m_valid = false;
    m_pts[1].m_valid = false;
}

void IntersectionSet::recompute() {
    compute();
    notify();
}

void IntersectionSet::setResults(const int count, DoublePair point1, DoublePair point2) {
    if (count == 2) {
        if (point1.x > point2.x || (point1.x == point2.x && point1.y > point2.y)) {
            std::swap(point1.x, point2.x);
            std::swap(point1.y, point2.y);
        }
    }

    switch (count) {
        case 2:
            m_pts[0].m_valid = true;
            m_pts[0].moveTo(point1.x, point1.y);
            m_pts[1].m_valid = true;
            m_pts[1].moveTo(point2.x, point2.y);
            m_valid = true;
            break;
        case 1:
            m_pts[0].m_valid = true;
            m_pts[0].moveTo(point1.x, point1.y);
            m_pts[1].m_valid = false;
            m_valid = true;
            break;
        case 0:
            m_pts[0].m_valid = false;
            m_pts[1].m_valid = false;
            m_valid = false;
            break;
        default:
            throw std::invalid_argument("IntersectionSet can only represent 0, 1 or 2 intersection points");
    }
}
