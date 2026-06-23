#include "geometryHelper.h"

#include <cassert>
#include <stdexcept>

auto lineParametersFromPoints(const Point* point1, const Point* point2) -> LineParameters {
    assert(point1 != nullptr);
    assert(point2 != nullptr);
    assert(point1 != point2);

    const double pos_x1 = point1->x();
    const double pos_y1 = point1->y();
    const double pos_x2 = point2->x();
    const double pos_y2 = point2->y();

    return { .a=pos_y1 - pos_y2, .b=pos_x2 - pos_x1, .c=(pos_x1 * pos_y2) - (pos_x2 * pos_y1) };
}

auto linePointsFromParameters(const double param_a, const double param_b, const double param_c) -> LinePoints {
    if (param_a == 0 && param_b == 0) {
        throw std::invalid_argument("Either a or b must not be 0");
    }

    double pos_x1;
    double pos_x2;
    double pos_y1;
    double pos_y2;
    if (param_a != 0) {
        pos_y1 = 0;
        pos_x1 = -param_c / param_a;
        pos_y2 = 1;
        pos_x2 = -(param_b + param_c) / param_a;
    } else {
        pos_x1 = 0;
        pos_y1 = -param_c / param_b;
        pos_x2 = 1;
        pos_y2 = -(param_a + param_c) / param_b;
    }

    auto* point1 = new Point(pos_x1, pos_y1);
    auto* point2 = new Point(pos_x2, pos_y2);

    if (pos_x1 < pos_x2) {
        return { .p1=point1, .p2=point2 };
    } 
    if (pos_x1 > pos_x2) {
        return { .p1=point2, .p2=point1 };
    }  
    if (pos_y1 < pos_y2) {
        return { .p1=point1, .p2=point2 };
    }

    return { .p1=point2, .p2=point1 };
   
   
}

auto polygonPointsFormConvexPolygon(const std::vector<Point *>& points) -> bool {
    const size_t n_points = points.size();
    if (n_points < 3) {
        return false;
    }

    int sign = 0;
    bool found_turn = false;

    for (size_t i = 0; i < n_points; ++i) {
        const Point* param_a = points[i];
        const Point* param_b = points[(i+1) % n_points];
        const Point* param_c = points[(i+2) % n_points];

        const double cross =
            ((param_c->y() - param_b->y()) * (param_b->x() - param_a->x())) -
            ((param_b->y() - param_a->y()) * (param_c->x() - param_b->x()));

        if (cross != 0) {
            found_turn = true;
            if (sign == 0) {
                sign = (cross > 0) ? 1 : -1;
            } else if ((cross > 0 && sign < 0) || (cross < 0 && sign > 0)) {
                return false;
            }
        }
    }

    return found_turn;
}
