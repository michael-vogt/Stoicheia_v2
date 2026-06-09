#pragma once

#include <memory>

#include "GeoObject.h"
#include "Point.h"
#include "Structs.h"


class Circle : public GeoObject {
public:
    Circle(PointPairForCircle points);
    Circle(Point* center, double radius);

    [[nodiscard]] auto center() const -> Point* { return m_center; };
    [[nodiscard]] auto radiusPoint() const -> Point* { return m_radiusPoint; };
    [[nodiscard]] auto radius() const -> double { return m_radius; };

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObjectPair source) override;

private:
    Point* m_center = nullptr;
    Point* m_radiusPoint = nullptr;
    double m_radius = 0.0;
    double m_fixedRadius = 0.0;
    std::unique_ptr<Point> m_fixedRadiusPoint;
};
