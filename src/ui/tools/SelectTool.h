#pragma once
#include <memory>
#include <QMouseEvent>
#include <QRubberBand>

#include "Tool.h"
#include "geometry/Point.h"
#include "ui/ToolContext.h"
#include "ui/commands/CreatePointCommand.h"
#include "ui/commands/MovePointCommand.h"

class SelectTool : public Tool {
    Point* m_draggedPoint = nullptr;
    std::unique_ptr<MoveCommand> m_activeMove;
    QPointF m_dragOffset;
    bool m_rubberBanding = false;
    QPoint m_rubberStart;
    QRubberBand* m_rubberBand = nullptr;
    Point* m_mergeCandidate = nullptr;

    Point* pointAt(const QPointF& scenePos) const;
    Point* nearbyPoint(const QPointF& scenePos, Point* exclude) const;
    GeoGraphicsItem* itemAt(const QPointF& scenePos, const std::type_info& type) const;
    bool isDraggable(Point* p) const;

    void startRubberBand(const QPoint& viewPos);
    void updateRubberBand(const QPoint& viewPos);
    void finishRubberBand(const QPoint& viewPos);

    void setMergeCandidate(Point* candidate);

public:
    explicit SelectTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    QCursor cursor() const override;
};
