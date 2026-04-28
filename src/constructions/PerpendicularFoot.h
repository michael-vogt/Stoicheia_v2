#ifndef STOICHEIA_PERPENDICULARFOOT_H
#define STOICHEIA_PERPENDICULARFOOT_H
#include "geometry/Line.h"
#include "geometry/Point.h"


class PerpendicularFoot : public Point {
    Point* m_point;
    Line* m_line;

public:
    PerpendicularFoot(Point* point, Line* line);

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;
};


#endif //STOICHEIA_PERPENDICULARFOOT_H
