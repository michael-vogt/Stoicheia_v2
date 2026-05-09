#pragma once
#include "LinearObjectType.h"
#include "Tool.h"
#include "geometry/Point.h"
#include "ui/GeoGraphicsItem.h"

class CreateLineTool : public Tool {
    Point* m_firstPoint = nullptr;
    bool m_firstIsNew = false;
    QGraphicsLineItem* m_preview = nullptr;
    LinearObjectType m_type;
    QPointF m_lastScenePos;

    Point* pointAt(const QPointF& scenePos) const;
    void setType(const LinearObjectType type);
    void updatePreview(const QPointF& currentScreenPos);
    QLineF computePreviewLine(const QPointF& endPos) const;
    void removePreview();

public:
    explicit CreateLineTool(const ToolContext& ctx, LinearObjectType type = LinearObjectType::Line);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QCursor cursor() const override;
};
