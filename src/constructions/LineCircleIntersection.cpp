#include "LineCircleIntersection.h"
#include "../Constants.h"
#include "Structs.h"
#include <stdexcept>
#include <cmath>


using namespace Constants;

LineCircleIntersection::LineCircleIntersection(LinearObject *line, Circle *circle)
: m_line(line), m_circle(circle)
{
    if (m_line == nullptr || m_circle == nullptr) {
        throw std::invalid_argument("null argument");
    }

    m_line->addDependent(this);
    m_circle->addDependent(this);
    IntersectionSet::recompute();
}

void LineCircleIntersection::onSourceRemoved(GeoObject *src) {
    if (src == static_cast<GeoObject*>(m_line)) { m_line = nullptr; }
    if (src == static_cast<GeoObject*>(m_circle)) { m_circle = nullptr; }
    IntersectionSet::onSourceRemoved(src);
}

void LineCircleIntersection::replaceSource(GeoObjectPair source) {
    if (m_line == source.oldGeoObject) { m_line = static_cast<LinearObject*>(source.newGeoObject); }
    if (m_circle == source.oldGeoObject) { m_circle = static_cast<Circle*>(source.newGeoObject); }
}

void LineCircleIntersection::compute() {
    if (m_line == nullptr || m_circle == nullptr) {
        setResults(0);
        return;
    }

    const double center_x = m_circle->center()->x();
    const double center_y = m_circle->center()->y();
    const double radius = m_circle->radius();

    const double point1_x = m_line->point1()->x();
    const double point1_y = m_line->point1()->y();
    const double point2_x = m_line->point2()->x();
    const double point2_y = m_line->point2()->y();

    const double delta_x = point2_x - point1_x;
    const double delta_y = point2_y - point1_y;

    // Berechne die Koeffizienten der quadratischen Gleichung, die sich durch Einsetzen der Geraden- in die Kreisgleichung ergibt
    const double coeff_a = (delta_x*delta_x) + (delta_y*delta_y);
    if (coeff_a < NumericConstants::DOUBLE_EPS_SQ) { // degenerated line
        setResults(0);
        return;
    }

    const double coeff_b = 2.0 * ((delta_x * (point1_x - center_x)) + (delta_y * (point1_y - center_y)));
    const double coeff_c = ((point1_x - center_x) * (point1_x - center_x)) + ((point1_y - center_y) * (point1_y - center_y)) - (radius * radius);

    if (const double discriminant = (coeff_b*coeff_b) - (4*coeff_a*coeff_c); discriminant < -NumericConstants::DOUBLE_EPS) { // no intersection
        setResults(0);
    } else if (discriminant < NumericConstants::DOUBLE_EPS) {
        // Gerade ist Tangente
        const double param_t = -coeff_b / (2*coeff_a);
        setResults(1, {.x=point1_x + (param_t*delta_x), .y=point1_y + (param_t*delta_y)});
    } else {
        const double sqrt_discriminant = sqrt(discriminant);
        const double param_t1 = (-coeff_b - sqrt_discriminant) / (2*coeff_a);
        const double param_t2 = (-coeff_b + sqrt_discriminant) / (2*coeff_a);
        setResults(2, 
            {.x=point1_x + (param_t1*delta_x), .y=point1_y + (param_t1*delta_y)},
            {.x=point1_x + (param_t2*delta_x), .y=point1_y + (param_t2*delta_y)});
    }
}