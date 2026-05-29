#include "Polygon.h"

#include <algorithm>
#include <iostream>

#include "geometryHelper.h"

Polygon::Polygon(std::initializer_list<Point *> points) {
    if (points.size() < 3)
        throw std::invalid_argument("A polygon consists of at least 3 points!");
    if (!polygonPointsFormConvexPolygon(points))
        throw std::invalid_argument("Only konvex polygons can be represented.");
    for (auto obj : points) {
        m_points.push_back(obj);
        obj->addDependent(this);
    }
}

void Polygon::recompute() {
    if (!polygonPointsFormConvexPolygon(m_points)) {
        m_valid = false;
        return;
    }

    notify();
}

bool Polygon::equals(const GeoObject &other) const {
    auto p = dynamic_cast<const Polygon *>(&other);
    if (!p) return false;

    auto otherPoints = p->m_points;
    if (otherPoints.size() != m_points.size()) return false;

    for (auto obj : otherPoints) {
        if (!std::ranges::contains(m_points, obj)) return false;
    }

    return true;
}
