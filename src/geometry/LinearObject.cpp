#include "LinearObject.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "Structs.h"
#include "geometryHelper.h"


constexpr double eps = std::numeric_limits<double>::epsilon();

LinearObject::LinearObject(PointPairForLinearObject points)
: m_point1(points.point1), m_point2(points.point2)
{
    if (m_point1 == nullptr || m_point2 == nullptr) {
        throw std::invalid_argument("null point");
    }

    m_point1->addDependent(this);
    m_point2->addDependent(this);

    m_lineParameters = lineParametersFromPoints(m_point1, m_point2);
    LinearObject::recompute();
}

LinearObject::LinearObject(LineParameters params)
: m_lineParameters(params)
{
    auto [p1, p2] = linePointsFromParameters(params.a, params.b, params.c);
    p1->addDependent(this);
    p2->addDependent(this);
    m_point1 = p1;
    m_point2 = p2;
}

// Ein Punkt P liegt auf der Geraden, wenn für die Projektion Q auf die Linie gilt: P = Q
auto LinearObject::contains(DoublePair point) const -> bool {
    double param_t = projectParameter(point);
    if (!containsT(param_t)) {
        return false;
    }

    // Vektor zwischen Projektion auf Linie und tatsächlichem Punkt
    double delta_x = m_point1->x() + (param_t*dx()) - point.x;
    double delta_y = m_point1->y() + (param_t*dy()) - point.y;
    return std::sqrt((delta_x*delta_x) + (delta_y*delta_y)) < eps;
}

auto LinearObject::projectParameter(DoublePair point) const -> double {
    double ddx = dx();
    double ddy = dy();
    double len2 = (ddx*ddx) + (ddy*ddy);
    if (len2 < eps*eps) {
        return 0.0;
    }
    return (((point.x - m_point1->x()) * ddx) + ((point.y - m_point1->y()) * ddy)) / len2;
}

void LinearObject::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_point1)) { 
        m_point1 = nullptr;
    }
    if (src == static_cast<GeoObject*>(m_point2)) { 
        m_point2 = nullptr;
    }
}

void LinearObject::recompute() {
    if (m_point1 == nullptr || m_point2 == nullptr) {
        return;
    }

    m_lineParameters = lineParametersFromPoints(m_point1, m_point2);
    
    double ddx = m_point2->x() - m_point1->x();
    double ddy = m_point2->y() - m_point1->y();
    m_length = std::sqrt((ddx*ddx) + (ddy*ddy));
    recomputeCount++;
    notify();
}

void LinearObject::replaceSource(GeoObjectPair source) {
    if (m_point1 == source.oldGeoObject) { 
        m_point1 = static_cast<Point*>(source.newGeoObject);
    }
    if (m_point2 == source.oldGeoObject) { 
        m_point2 = static_cast<Point*>(source.newGeoObject);
    }
}