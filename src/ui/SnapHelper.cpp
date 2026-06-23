#include "SnapHelper.h"

#include "GeoPointItem.h"
#include "Grid.h"

SnapHelper::SnapHelper(QGraphicsScene *scene, const Grid* grid)
: m_scene(scene), m_grid(grid)
{}

auto SnapHelper::snap(const QPointF &scenePos, bool snapActive) const -> QPointF {
    // Punkt-Snap hat Vorrang vor Grid-Snap
    if (Point* point = snapToPoint(scenePos)) {
        return {point->x(), point->y()};
    }

    //return snapToGrid(scenePos);
    return m_grid->snap(scenePos, snapActive);
}

auto SnapHelper::snapToGrid(const QPointF &scenePos) const -> QPointF {
    double pos_x = std::round(scenePos.x() / m_gridSpacing) * m_gridSpacing;
    double pos_y = std::round(scenePos.y() / m_gridSpacing) * m_gridSpacing;
    return {pos_x, pos_y};
}

auto SnapHelper::snapToPoint(const QPointF &scenePos) const -> Point * {
    Point* nearest = nullptr;
    double min_dist = m_snapRadius * m_snapRadius;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_snapRadius, m_snapRadius),
            QSizeF(m_snapRadius * 2, m_snapRadius * 2)));

    for (QGraphicsItem* item : items) {
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            Point* point = point_item->point();
            double delta_x = point->x() - scenePos.x();
            double delta_y = point->y() - scenePos.y();
            double dist2 = (delta_x*delta_x) + (delta_y*delta_y);
            if (dist2 < min_dist) {
                min_dist = dist2;
                nearest = point;
            }
        }
    }
    return nearest;
}
