#ifndef STOICHEIA_CIRCLE_H
#define STOICHEIA_CIRCLE_H
#include <memory>

#include "GeoObject.h"
#include "Point.h"


class Circle : public GeoObject {
    Point* m_center = nullptr;
    Point* m_radiusPoint = nullptr;
    double m_radius = 0.0;
    double m_fixedRadius = 0.0;
    std::unique_ptr<Point> m_fixedRadiusPoint;

public:
    Circle(Point* center, Point* radiusPoint);
    Circle(Point* center, double radius);

    Point* center() const;
    double radius() const;

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;
};


#endif //STOICHEIA_CIRCLE_H
