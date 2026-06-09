#pragma once
#include <QGraphicsScene>

#include "geometry/GeoObject.h"

class Point;
class LinearObject;
class Circle;

constexpr double DEFAULT_HITTEST_TOLERANCE = 8;

class HitTest {
public:
    explicit HitTest(QGraphicsScene* scene, double tolerance = DEFAULT_HITTEST_TOLERANCE);

    // nächster Punkt in Reichweite, oder nullptr
    [[nodiscard]] auto pointAt(const QPointF& scenePos) const -> Point*;

    // nächstes LinearObject in Reichweite, oder nullptr
    [[nodiscard]] auto linearObjectAt(const QPointF& scenePos) const -> LinearObject*;

    // nächster Kreis in Reichweite, oder nullptr
    [[nodiscard]] auto circleAt(const QPointF& scenePos) const -> Circle*;

    // nächstes beliebiges GeoObject in Reichweite (Punkt hat Vorrang), oder nullptr
    [[nodiscard]] auto anyObjectAt(const QPointF& scenePos) const -> GeoObject*;

    void setTolerance(const double tolerance) { m_tolerance = tolerance; }

private:
    QGraphicsScene* m_scene;
    double m_tolerance;
};
