#pragma once
#include "Tool.h"


class CreateCircleTool : public Tool {
    Point* m_centerPoint = nullptr;
    bool m_centerIsNew = false;
    QPointF m_centerScenePos;
    QGraphicsEllipseItem* m_preview = nullptr;

    QRectF computePreviewCircle(const QPointF& scenePos) const;
    Point* pointAt(const QPointF& scenePos) const;
    void removePreview();

public:
    explicit CreateCircleTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QCursor cursor() const override;
};
