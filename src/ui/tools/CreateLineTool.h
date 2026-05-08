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

    Point* pointAt(const QPointF& scenePos) const;
    void removePreview();

public:
    explicit CreateLineTool(const ToolContext& ctx, LinearObjectType type = LinearObjectType::Line);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QCursor cursor() const override;
};
