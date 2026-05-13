#pragma once
#include "Tool.h"

#include <QPointF>

class CreatePointTool : public Tool {
    QGraphicsEllipseItem* m_preview = nullptr;
    static constexpr double RADIUS = 4.0;

    void updatePreview(const QPointF& scenePos);
    void removePreview();

public:
    explicit CreatePointTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QCursor cursor() const override;
};
