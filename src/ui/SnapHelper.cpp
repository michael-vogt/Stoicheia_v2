#include "SnapHelper.h"

#include "GeoPointItem.h"
#include "Grid.h"

SnapHelper::SnapHelper(QGraphicsScene *scene, const Grid* grid)
: m_scene(scene), m_grid(grid)
{}

QPointF SnapHelper::snap(const QPointF &scenePos, bool snapActive) const {
    // Punkt-Snap hat Vorrang vor Grid-Snap
    if (Point* p = snapToPoint(scenePos))
        return QPointF(p->x(), p->y());

    //return snapToGrid(scenePos);
    return m_grid->snap(scenePos, snapActive);
}

QPointF SnapHelper::snapToGrid(const QPointF &scenePos) const {
    double x = std::round(scenePos.x() / m_gridSpacing) * m_gridSpacing;
    double y = std::round(scenePos.y() / m_gridSpacing) * m_gridSpacing;
    return QPointF(x, y);
}

Point *SnapHelper::snapToPoint(const QPointF &scenePos) const {
    Point* nearest = nullptr;
    double minDist = m_snapRadius * m_snapRadius;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_snapRadius, m_snapRadius),
            QSizeF(m_snapRadius * 2, m_snapRadius * 2)));

    for (QGraphicsItem* item : items) {
        if (auto* pi = dynamic_cast<GeoPointItem*>(item)) {
            Point* p = pi->point();
            double dx = p->x() - scenePos.x();
            double dy = p->y() - scenePos.y();
            double d2 = dx*dx + dy*dy;
            if (d2 < minDist) {
                minDist = d2;
                nearest = p;
            }
        }
    }
    return nearest;
}
