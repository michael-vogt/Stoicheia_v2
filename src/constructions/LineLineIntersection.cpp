#include "LineLineIntersection.h"

LineLineIntersection::LineLineIntersection(Line *line1, Line *line2) : m_line1(line1), m_line2(line2) {
    if (m_line1 == nullptr || m_line2 == nullptr)
        throw std::invalid_argument("null line");
    m_line1->addDependent(this);
    m_line2->addDependent(this);
    IntersectionSet::recompute();
}

void LineLineIntersection::compute() {
    if (m_line1 == nullptr || m_line2 == nullptr) {
        setResults(0);
        return;
    }

    auto [a1, b1, c1] = m_line1->parameters();
    auto [a2, b2, c2] = m_line2->parameters();

    const double det = a1 * b2 - b1 * a2;
    if (std::abs(det) < 1e-10) {
        setResults(0);
        return;
    }

    setResults(1, (b1 * c2 - c1 * b2) / det, (c1 * a2 - a1 * c2) / det);
}

void LineLineIntersection::onSourceRemoved(GeoObject *src) {
    if (src == static_cast<GeoObject*>(m_line1)) m_line1 = nullptr;
    if (src == static_cast<GeoObject*>(m_line2)) m_line2 = nullptr;
    IntersectionSet::onSourceRemoved(src);
}
