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

    bool snapActive = event->modifiers() & Qt::AltModifier;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    m_ctx.commandStack->execute(std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y()));
    event->accept();
}

void CreatePointTool::mouseMoveEvent(QMouseEvent *event) {
    // Vorschau-Punkt könnte hier gezeichnet werden. Vorerst leer
    event->ignore();
}
