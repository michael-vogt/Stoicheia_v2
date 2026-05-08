#include "CreatePointTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreatePointCommand.h"

CreatePointTool::CreatePointTool(const ToolContext &ctx) : Tool(ctx) {}

void CreatePointTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
}

QCursor CreatePointTool::cursor() const {
    return Qt::CrossCursor;
}

void CreatePointTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    QPointF pos = m_ctx.drawingBoard->mapToScene(event->pos());
    m_ctx.commandStack->execute(std::make_unique<CreatePointCommand>(m_ctx.adapter, pos.x(), pos.y()));
    event->accept();
}

void CreatePointTool::mouseMoveEvent(QMouseEvent *event) {
    // Vorschau-Punkt könnte hier gezeichnet werden. Vorerst leer
    event->ignore();
}
