#pragma once
#include "Tool.h"


class CreateCircleTool : public Tool {

    Q_OBJECT

public:
    explicit CreateCircleTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    [[nodiscard]] auto cursor() const -> QCursor override { return Qt::CrossCursor; }

private:
    [[nodiscard]] auto computePreviewCircle(const QPointF& scenePos) const -> QRectF;
    [[nodiscard]] auto pointAt(const QPointF& scenePos) const -> Point*;
    void removePreview();

    Point* m_centerPoint = nullptr;
    bool m_centerIsNew = false;
    QPointF m_centerScenePos;
    QGraphicsEllipseItem* m_preview = nullptr;
};
