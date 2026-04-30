#include "IntersectionSet.h"

IntersectionSet::IntersectionSet() {
    this->addDependent(&m_pts[0]);
    this->addDependent(&m_pts[1]);
}

void IntersectionSet::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    m_pts[0].m_valid = false;
    m_pts[1].m_valid = false;
}

void IntersectionSet::recompute() {
    compute();
    notify();
}

void IntersectionSet::setResults(const int count, double x1, double y1, double x2, double y2) {
    if (count == 2) {
        if (x1 > x2 || (x1 == x2 && y1 > y2)) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
    }

    switch (count) {
        case 2:
            m_pts[0].m_valid = true;
            m_pts[0].moveTo(x1, y1);
            m_pts[1].m_valid = true;
            m_pts[1].moveTo(x2, y2);
            m_valid = true;
            break;
        case 1:
            m_pts[0].m_valid = true;
            m_pts[0].moveTo(x1, y1);
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

std::string IntersectionSet::toString() {
    return "";
}
