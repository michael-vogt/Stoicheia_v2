#pragma once
#include <vector>

#include "GeoObject.h"
#include "Point.h"


class Polygon : public GeoObject {
    std::vector<Point*> m_points;
public:
    Polygon(std::initializer_list<Point*> points);

    std::vector<Point*> points() const { return m_points; };

    void recompute() override;
    std::string toString() override { return ""; }
};
