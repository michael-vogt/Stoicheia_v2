#pragma once

#include "IntersectionSet.h"
#include "../geometry/Circle.h"
#include <cmath>

// Schnittpunkte zweier Kreise (0, 1 oder 2 Punkte).
//
// Mathematik:
//   Zwei Kreise M1/r1 und M2/r2. Verbindungslinie M1M2 hat Länge d.
//   Abstand vom Mittelpunkt des Schnittpunkt-Segments zu M1:
//     a = (d² + r1² - r2²) / (2d)
//   Halbhöhe des Segments:
//     h = sqrt(r1² - a²)
//   Schnittpunkte: M1 + a*(M2-M1)/d  ±  h * Normalenvektor
class CircleCircleIntersection : public IntersectionSet {
    Circle* m_c1 = nullptr;
    Circle* m_c2 = nullptr;
public:
    CircleCircleIntersection(Circle* c1, Circle* c2);

    void onSourceRemoved(GeoObject* src) override;

protected:
    void compute() override;
};