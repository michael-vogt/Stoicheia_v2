#pragma once
#include "Tool.h"
#include "geometry/Point.h"
#include "ui/GeoGraphicsItem.h"

class CreateLineTool : public Tool {
    Point* m_firstPoint = nullptr;
    bool m_firstIsNew = false;
    QGraphicsLineItem* m_preview = nullptr;

    Point* pointAt(const QPointF& scenePos) const;
    void removePreview();

public:
    explicit CreateLineTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QCursor cursor() const override;
};
