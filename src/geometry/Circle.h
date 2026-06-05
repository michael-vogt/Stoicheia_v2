#pragma once

#include <memory>

#include "GeoObject.h"
#include "Point.h"


class Circle : public GeoObject {
public:
    Circle(Point* center, Point* radiusPoint);
    Circle(Point* center, double radius);

    Point* center() const { return m_center; };
    Point* radiusPoint() const { return m_radiusPoint; };
    double radius() const { return m_radius; };

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;

private:
    Point* m_center = nullptr;
    Point* m_radiusPoint = nullptr;
    double m_radius = 0.0;
    double m_fixedRadius = 0.0;
    std::unique_ptr<Point> m_fixedRadiusPoint;
};
