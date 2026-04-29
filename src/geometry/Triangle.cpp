#include "Triangle.h"

#include <stdexcept>

Triangle::Triangle(LinearObject *a, LinearObject *b, LinearObject *c) : m_a(a), m_b(b), m_c(c) {
    if (m_a == nullptr || m_b == nullptr || m_c == nullptr)
        throw std::invalid_argument("null point");
    m_a->addDependent(this);
    m_b->addDependent(this);
    m_c->addDependent(this);
    Triangle::recompute();
}

double Triangle::perimeter() const {
    return m_perimater;
}

void Triangle::onSourceRemoved(GeoObject *src) {
    m_valid = false;
}

void Triangle::recompute() {
    m_perimater = (m_a ? m_a->length() : 0)
                + (m_b ? m_b->length() : 0)
                + (m_c ? m_c->length() : 0);
    recomputeCount++;
    notify();
}

std::string Triangle::toString() {
    return "";
}
