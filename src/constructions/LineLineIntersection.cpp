#include "LineLineIntersection.h"
#include "Structs.h"
#include <limits>
#include <stdexcept>

constexpr double eps = std::numeric_limits<double>::epsilon();

LineLineIntersection::LineLineIntersection(LinearObject *line1, LinearObject *line2)
: m_line1(line1), m_line2(line2)
{
    if (m_line1 == nullptr || m_line2 == nullptr) {
        throw std::invalid_argument("null line");
    }
    m_line1->addDependent(this);
    m_line2->addDependent(this);
    IntersectionSet::recompute();
}

void LineLineIntersection::onSourceRemoved(GeoObject *src) {
    if (src == static_cast<GeoObject*>(m_line1)) { m_line1 = nullptr; }
    if (src == static_cast<GeoObject*>(m_line2)) { m_line2 = nullptr; }
    IntersectionSet::onSourceRemoved(src);
}

void LineLineIntersection::replaceSource(GeoObjectPair source) {
    if (m_line1 == source.oldGeoObject) { m_line1 = static_cast<LinearObject*>(source.newGeoObject); }
    if (m_line2 == source.oldGeoObject) { m_line2 = static_cast<LinearObject*>(source.newGeoObject); }
}

void LineLineIntersection::compute() {
    if (m_line1 == nullptr || m_line2 == nullptr) {
        setResults(0);
        return;
    }

    auto [line1_p1, line1_p2] = m_line1->points();
    auto [line2_p1, line2_p2] = m_line2->points();

    DoublePair dir1 = {.x = line1_p2->x() - line1_p1->x(), .y = line1_p2->y() - line1_p1->y()};
    DoublePair dir2 = {.x = line2_p2->x() - line2_p1->x(), .y = line2_p2->y() - line2_p1->y()};

    double det = (dir1.x * dir2.y) - (dir1.y * dir2.x);
    if (det < eps) {
        setResults(0);
        return;
    }

    DoublePair p1p2 = {.x=line1_p1->x() - line2_p1->x(), .y=line1_p1->y() - line2_p1->y()};
    double param_t = (p1p2.x*dir2.y) - (p1p2.y*dir2.x);

    setResults(1, {.x=line1_p1->x() + (param_t*dir1.x), .y=line1_p1->x() + (param_t*dir1.y)});
}