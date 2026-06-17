#pragma once

#include <QRubberBand>
#include <qevent.h>

#include "Tool.h"
#include "geometry/Point.h"
#include "ui/GeoGraphicsItem.h"
#include "ui/ToolContext.h"
#include "ui/commands/MoveCommand.h"


class SelectTool : public Tool {

    Q_OBJECT

public:
    explicit SelectTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    [[nodiscard]] auto cursor() const -> QCursor override { return !m_draggedPoints.empty() ? Qt::ClosedHandCursor : Qt::ArrowCursor; }

private:
    void showContextMenu(QMouseEvent* event, GeoObject* obj);
    [[nodiscard]] auto pointAt(const QPointF& scenePos) const -> Point*;
    auto nearbyPoint(const QPointF& scenePos, Point* exclude) const -> Point*;
    [[nodiscard]] auto itemAt(const QPointF& scenePos, const std::type_info& type) const -> GeoGraphicsItem*;
    static auto isDraggable(Point* point) -> bool  { return typeid(*point) == typeid(Point); }
    void startMultiDrag(const QPointF& scenePos);
    void startRubberBand(const QPoint& viewPos);
    void updateRubberBand(const QPoint& viewPos);
    void finishRubberBand(const QPoint& viewPos);
    void setMergeCandidate(Point* candidate);
    void setConstrainCandidate(GeoObject* candidate);

    std::vector<std::unique_ptr<MoveCommand>> m_activeMoves;
    std::vector<Point*> m_draggedPoints;
    QPointF m_dragOffset;
    bool m_rubberBanding = false;
    QPoint m_rubberStart;
    QRubberBand* m_rubberBand = nullptr;
    Point* m_mergeCandidate = nullptr;
    GeoObject* m_constrainCandidate = nullptr; // Gerade oder Kreis für Einschränkung
};
