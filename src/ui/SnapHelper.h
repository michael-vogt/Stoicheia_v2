#pragma once
#include <QGraphicsScene>

#include "geometry/Point.h"
#include "Grid.h"
#include "./dialogs/AppSettings.h"


constexpr double DEFAULT_SNAPHELPER_SNAPRADIUS = 15;

class SnapHelper {
public:
    explicit SnapHelper(QGraphicsScene* scene, const Grid* grid);

    [[nodiscard]] auto snap(const QPointF& scenePos, bool snapActive) const -> QPointF;
    [[nodiscard]] auto snapToGrid(const QPointF& scenePos) const -> QPointF;
    [[nodiscard]] auto snapToPoint(const QPointF& scenePos) const -> Point*;

    void setGridSpacing(double gridSpacing) { m_gridSpacing = gridSpacing; }
    void setSnapRadius(double snapRadius) { m_snapRadius = snapRadius; }

private:
    QGraphicsScene* m_scene;
    const Grid* m_grid;
    double m_gridSpacing = DEFAULT_GRID_SPACING;
    double m_snapRadius = DEFAULT_SNAPHELPER_SNAPRADIUS;
};
