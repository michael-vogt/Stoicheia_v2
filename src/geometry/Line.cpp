#include "Line.h"

#include <cassert>
#include <cmath>
#include <format>
#include <iostream>

#include "geometryHelper.h"

/*
 * Constructors and destructor
 */
Line::Line(Point *p1, Point *p2) {
    assert(p1 != nullptr);
    assert(p2 != nullptr);

    p1->addDependent(this);
    p2->addDependent(this);

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

    auto [a, b, c] = lineParametersFromPoints(m_p1, m_p2);
    m_a = a;
    m_b = b;
    m_c = c;
}

Line::Line(const double a, const double b, const double c) {
    m_a = a;
    m_b = b;
    m_c = c;

    auto [p1, p2] = linePointsFromParameters(a, b, c);
    p1->addDependent(this);
    p2->addDependent(this);
    m_p1 = p1;
    m_p2 = p2;
}

/*
 * Getter
 */
double Line::getLength() const {
    double dx = m_p1->x() - m_p2->x();
    double dy = m_p1->y() - m_p2->y();

    return std::sqrt(dx*dx+dy*dy);
}

Point *Line::p1() const {
    return m_p1;
}

Point *Line::p2() const {
    return m_p2;
}

LineParameters Line::parameters() const {
    return { m_a, m_b, m_c };
}

LinePoints Line::points() const {
    return { m_p1, m_p2 };
}

/*
 * Overridden methods
 */
void Line::recompute() {
    auto [a, b, c] = lineParametersFromPoints(m_p1, m_p2);
    m_a = a;
    m_b = b;
    m_c = c;
    recomputeCount++;
    notify();
}

std::string Line::toString() {
    std::string equation = std::format("0 = {}x + {}y + {}", m_a, m_b, m_c);
    std::string str = std::format("[{}; P1{}--P2{}]", equation, m_p1->toString(), m_p2->toString()); //"Line " + m_p1->toString() + " -- " + m_p2->toString();
    return str;
}
