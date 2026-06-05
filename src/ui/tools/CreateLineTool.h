#pragma once
#include "LinearObjectType.h"
#include "Tool.h"
#include "geometry/Point.h"
#include "ui/GeoGraphicsItem.h"
#include "ui/commands/CreatePointCommand.h"

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
    Point* pointAt(const QPointF& scenePos) const;
    void setType(const LinearObjectType type);
    QLineF computePreviewLine(const QPointF& endPos) const;
    void removePreview();

    LinearObjectType m_type;
    Point* m_firstPoint = nullptr;
    QPointF m_firstScenePos;
    bool m_firstIsNew = false;
    QGraphicsLineItem* m_preview = nullptr;
    QPointF m_lastScenePos;
};
