#pragma once

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
LineParameters lineParametersFromPoints(const Point* p1, const Point* p2);
LinePoints linePointsFromParameters(double a, double b, double c);
