#pragma once
#include <QGraphicsScene>

#include "geometry/GeoObject.h"

class Point;
class LinearObject;
class Circle;

class HitTest {
public:
    explicit HitTest(QGraphicsScene* scene, double tolerance = 8.0);

    // nächster Punkt in Reichweite, oder nullptr
    Point* pointAt(const QPointF& scenePos) const;

    // nächstes LinearObject in Reichweite, oder nullptr
    LinearObject* linearObjectAt(const QPointF& scenePos) const;

    // nächster Kreis in Reichweite, oder nullptr
    Circle* circleAt(const QPointF& scenePos) const;

    // nächstes beliebiges GeoObject in Reichweite (Punkt hat Vorrang), oder nullptr
    GeoObject* anyObjectAt(const QPointF& scenePos) const;

    void setTolerance(const double tolerance) { m_tolerance = tolerance; }

private:
    QGraphicsScene* m_scene;
    double m_tolerance;
};
