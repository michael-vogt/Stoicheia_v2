#pragma once

#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"


class PerpendicularFoot : public Point {
    Point* m_point;
    LinearObject* m_line;

public:
    PerpendicularFoot(Point* point, LinearObject* line);

    Point* point() const { return m_point; }
    LinearObject* line() const { return m_line; }

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;

    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;
};