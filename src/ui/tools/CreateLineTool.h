#pragma once
#include "LinearObjectType.h"
#include "Tool.h"
#include "geometry/Point.h"


class CreateLineTool : public Tool {

    Q_OBJECT

public:
    explicit CreateLineTool(const ToolContext& ctx, LinearObjectType type = LinearObjectType::Line);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QCursor cursor() const override { return Qt::CrossCursor; }

private:
    [[nodiscard]] auto pointAt(const QPointF& scenePos) const -> Point*;
    void setType(LinearObjectType type);
    [[nodiscard]] auto computePreviewLine(const QPointF& endPos) const -> QLineF;
    void removePreview();

    LinearObjectType m_type;
    Point* m_firstPoint = nullptr;
    QPointF m_firstScenePos;
    bool m_firstIsNew = false;
    QGraphicsLineItem* m_preview = nullptr;
    QPointF m_lastScenePos;
};
