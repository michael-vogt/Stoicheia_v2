#pragma once
#include <QGraphicsScene>

#include "geometry/Point.h"
#include "Grid.h"

class SnapHelper {
public:
    explicit SnapHelper(QGraphicsScene* scene, const Grid* grid);

    QPointF snap(const QPointF& scenePos, bool snapActive) const;
    QPointF snapToGrid(const QPointF& scenePos) const;
    Point* snapToPoint(const QPointF& scenePos) const;

    void setGridSpacing(double gridSpacing) { m_gridSpacing = gridSpacing; }
    void setSnapRadius(double snapRadius) { m_snapRadius = snapRadius; }

private:
    QGraphicsScene* m_scene;
    const Grid* m_grid;
    double m_gridSpacing = 50.0;
    double m_snapRadius = 15.0;
};
