#include "LinearObject.h"

#include <cassert>
#include <cmath>
#include <format>
#include <iostream>

#include "geometryHelper.h"

/*
 * Constructors and destructor
 */
LinearObject::LinearObject(Point *p1, Point *p2) : m_p1(p1), m_p2(p2) {
    if (m_p1 == nullptr || m_p2 == nullptr)
        throw std::invalid_argument("null point");

    if (p1->x() < p2->x()) {
        m_p1 = p1;
        m_p2 = p2;
    } else if (p1->x() > p2->x()) {
        m_p1 = p2;
        m_p2 = p1;
    } else {
        if (p1->y() < p2->y()) {
            m_p1 = p1;
            m_p2 = p2;
        } else {
            m_p1 = p2;
            m_p2 = p1;
        }
    }

    m_p1->addDependent(this);
    m_p2->addDependent(this);

    auto [a, b, c] = lineParametersFromPoints(m_p1, m_p2);
    m_a = a;
    m_b = b;
    m_c = c;

    LinearObject::recompute();
}

LinearObject::LinearObject(const double a, const double b, const double c) {
    m_a = a;
    m_b = b;
    m_c = c;

    auto [p1, p2] = linePointsFromParameters(a, b, c);
    p1->addDependent(this);
    p2->addDependent(this);
    m_p1 = p1;
    m_p2 = p2;
}

bool LinearObject::contains(double px, double py, double eps) const {
    double t = projectParameter(px, py);
    if (!containsT(t))
        return false;

    double qx = m_p1->x() + t*dx() - px;
    double qy = m_p1->y() + t*dy() - py;
    return std::sqrt(qx*qx + qy*qy) < eps;
}

/*
 * Getter
 */
double LinearObject::dx() const {
    return (m_p1 != nullptr && m_p2 != nullptr) ? m_p2->x() - m_p1->x() : 0;
}

double LinearObject::dy() const {
    return (m_p1 != nullptr && m_p2 != nullptr) ? m_p2->y() - m_p1->y() : 0;
}

double LinearObject::length() const {
    return m_length;
}

Point *LinearObject::p1() const {
    return m_p1;
}

Point *LinearObject::p2() const {
    return m_p2;
}

LineParameters LinearObject::parameters() const {
    return { m_a, m_b, m_c };
}

LinePoints LinearObject::points() const {
    return { m_p1, m_p2 };
}

double LinearObject::projectParameter(double px, double py) const {
    double ddx = dx();
    double ddy = dy();
    double len2 = ddx*ddx + ddy*ddy;
    if (len2 < 1e-20) return 0.0;
    return ((px - m_p1->x()) * ddx + (py - m_p1->y()) * ddy) / len2;
}

/*
 * Overridden methods
 */
void LinearObject::onSourceRemoved(GeoObject *src) {
    m_valid = false;
    if (src == static_cast<GeoObject*>(m_p1)) m_p1 = nullptr;
    if (src == static_cast<GeoObject*>(m_p2)) m_p2 = nullptr;
}

void LinearObject::recompute() {
    if (m_p1 == nullptr || m_p2 == nullptr)
        return;

    auto [a, b, c] = lineParametersFromPoints(m_p1, m_p2);
    m_a = a;
    m_b = b;
    m_c = c;

    double ddx = m_p2->x() - m_p1->x();
    double ddy = m_p2->y() - m_p1->y();
    m_length = std::sqrt(ddx*ddx + ddy*ddy);
    recomputeCount++;
    notify();
}

std::string LinearObject::toString() {
    std::string equation = std::format("0 = {}x + {}y + {}", m_a, m_b, m_c);
    std::string str = std::format("[{}; P1{}--P2{}]", equation, m_p1->toString(), m_p2->toString()); //"Line " + m_p1->toString() + " -- " + m_p2->toString();
    return str;
}
