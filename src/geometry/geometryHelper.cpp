#include "geometryHelper.h"

#include <cassert>
#include <stdexcept>

LineParameters lineParametersFromPoints(const Point* p1, const Point* p2) {
    assert(p1 != nullptr);
    assert(p2 != nullptr);
    assert(p1 != p2);

    const double x1 = p1->x();
    const double y1 = p1->y();
    const double x2 = p2->x();
    const double y2 = p2->y();

    return { y1 - y2, x2 - x1, x1 * y2 - x2 * y1 };
}

LinePoints linePointsFromParameters(const double a, const double b, const double c) {
    if (a == 0 && b == 0)
        throw std::invalid_argument("Either a or b must not be 0");

    double x1, x2, y1, y2;
    if (a != 0) {
        y1 = 0;
        x1 = -c / a;
        y2 = 1;
        x2 = -(b + c) / a;
    } else {
        x1 = 0;
        y1 = -c / b;
        x2 = 1;
        y2 = -(a + c) / b;
    }

    auto* p1 = new Point(x1, y1);
    auto* p2 = new Point(x2, y2);

    if (x1 < x2) {
        return { p1, p2 };
    } else if (x1 > x2) {
        return { p2, p1 };
    } else {
        if (y1 < y2) {
            return { p1, p2 };
        } else {
            return { p2, p1 };
        }
    }
}
