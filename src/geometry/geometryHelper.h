#pragma once

#include <vector>

#include "Point.h"

/*
 * Structs
 */
struct LineParameters {
    double a;
    double b;
    double c;
};

struct LinePoints {
    Point* p1;
    Point* p2;
};

/*
 * Functions
 */
LineParameters lineParametersFromPoints(const Point* point1, const Point* point2);
LinePoints linePointsFromParameters( double param_a,  double param_b,  double param_c);

bool polygonPointsFormConvexPolygon(const std::vector<Point*>& points);
