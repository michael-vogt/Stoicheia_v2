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
}

void LineLineIntersection::onSourceRemoved(GeoObject *src) {
    if (src == static_cast<GeoObject*>(m_line1)) m_line1 = nullptr;
    if (src == static_cast<GeoObject*>(m_line2)) m_line2 = nullptr;
    IntersectionSet::onSourceRemoved(src);
}
