#pragma once
#include "Tool.h"

#include <QPointF>

class CreatePointTool : public Tool {

    Q_OBJECT

public:
    explicit CreatePointTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    [[nodiscard]] auto cursor() const -> QCursor override { return Qt::CrossCursor; }

private:
    void updatePreview(const QPointF& scenePos);
    void removePreview();

    QGraphicsEllipseItem* m_preview = nullptr;
};
