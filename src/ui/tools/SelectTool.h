#pragma once
#include <memory>
#include <QMouseEvent>

#include "Tool.h"
#include "geometry/Point.h"
#include "ui/ToolContext.h"
#include "ui/commands/CreatePointCommand.h"
#include "ui/commands/MovePointCommand.h"

class SelectTool : public Tool {
    Point* m_draggedPoint = nullptr;
    std::unique_ptr<MoveCommand> m_activeMove;
    QPointF m_dragOffset;
    Point* pointAt(const QPointF& scenePos) const;

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
