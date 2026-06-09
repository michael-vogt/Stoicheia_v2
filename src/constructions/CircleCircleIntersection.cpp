#include "CircleCircleIntersection.h"
#include "Structs.h"
#include <cmath>
#include <limits>
#include <stdexcept>

constexpr double eps = std::numeric_limits<double>::epsilon();

CircleCircleIntersection::CircleCircleIntersection(Circle *circle1, Circle *circle2)
: m_circle1(circle1), m_circle2(circle2)
{
    if ((circle1 == nullptr) || (circle2 == nullptr)) { throw std::invalid_argument("null argument"); }
    circle1->addDependent(this);
    circle2->addDependent(this);
    IntersectionSet::recompute();
}

void CircleCircleIntersection::onSourceRemoved(GeoObject *src)  {
    if (src == static_cast<GeoObject*>(m_circle1)) { m_circle1 = nullptr; }
    if (src == static_cast<GeoObject*>(m_circle2)) { m_circle2 = nullptr; }
    IntersectionSet::onSourceRemoved(src);
}

void CircleCircleIntersection::replaceSource(GeoObjectPair source) {
    if (m_circle1 == source.oldGeoObject) { m_circle1 = static_cast<Circle*>(source.newGeoObject); }
    if (m_circle2 == source.oldGeoObject) { m_circle2 = static_cast<Circle*>(source.newGeoObject); }
}

void CircleCircleIntersection::compute()  {
    if ((m_circle1 == nullptr) || (m_circle2 == nullptr)) { setResults(0); return; }

    double circle1_x = m_circle1->center()->x();
    double circle1_y = m_circle1->center()->y();
    double circle2_x = m_circle2->center()->x();
    double circle2_y = m_circle2->center()->y();
    double circle1_r = m_circle1->radius();
    double circle2_r = m_circle2->radius();

    double delta_x = circle2_x - circle1_x;
    double delta_y = circle2_y - circle1_y;
    double delta = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));

    // Keine Schnittpunkte: zu weit auseinander oder einer im anderen
    if (delta > circle1_r + circle2_r + eps || delta < std::abs(circle1_r - circle2_r) - eps) {
        setResults(0); return;
    }
    // Identische Kreise
    if (delta < eps && std::abs(circle1_r - circle2_r) < eps) {
        setResults(0); return; // unendlich viele – nicht darstellbar
    }

    // Abstand der Schnittpunkte vom Mittelpunkt des ersten Kreises
    double dist1_x = ((delta * delta) + (circle1_r * circle1_r) - (circle2_r * circle2_r)) / (2*delta);    
    double dist1_y_sq = (circle1_r * circle1_r) - (dist1_x * dist1_x);

    // Projektion der Schnittpunkte auf Verbindungslinie
    double proj_x = circle1_x + (dist1_x * delta_x/delta);
    double proj_y = circle1_y + (dist1_x * delta_y/delta);

    
    // Abstand der Schnittpunkte = 2 * dist1_y
    if (4 * dist1_y_sq < (eps * eps)) {
        // Tangente: ein Schnittpunkt
        setResults(1, {.x=proj_x, .y=proj_y});
    } else {
        double dist1_y = std::sqrt(dist1_y_sq);
        // Normalenvektor: senkrecht zu (delta_x,delta_y)
        double normal_x = -delta_y/delta;
        double normal_y = delta_x/delta;
        setResults(2,
            {.x=proj_x + (dist1_y * normal_x), .y=proj_y + (dist1_y * normal_y)},
            {.x=proj_x - (dist1_y * normal_x), .y=proj_y - (dist1_y * normal_y)});
    }
}