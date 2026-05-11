#pragma once
#include <QGraphicsScene>

#include "geometry/Point.h"

class SnapHelper {
    QGraphicsScene* m_scene;
    double m_gridSpacing = 50.0;
    double m_snapRadius = 15.0;

public:
    explicit SnapHelper(QGraphicsScene* scene, double gridSpacing = 50.0);

    QPointF snap(const QPointF& scenePos, bool snapActive) const;
    QPointF snapToGrid(const QPointF& scenePos) const;

    Point* snapToPoint(const QPointF& scenePos) const;

    void setGridSpacing(double gridSpacing) { m_gridSpacing = gridSpacing; }
    void setSnapRadius(double snapRadius) { m_snapRadius = snapRadius; }
};
