#include "CreateMidpointTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreateMidpointCommand.h"
#include <cmath>

CreateMidpointTool::CreateMidpointTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreateMidpointTool::onActivate() {
    showStatus(QObject::tr("Ersten Punkt klicken"));
}

void CreateMidpointTool::onCancel() {
    m_firstPoint = nullptr;
}

void CreateMidpointTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    Point*  hit      = m_ctx.hitTest->pointAt(scenePos);

    if (!hit) { event->accept(); return; }

    if (!m_firstPoint) {
        m_firstPoint = hit;
        highlightObject(m_firstPoint, true);
        showStatus(QObject::tr("Zweiten Punkt klicken"));
    } else {
        if (hit != m_firstPoint) {
            m_ctx.commandStack->execute(
                std::make_unique<CreateMidpointCommand>(
                    m_ctx.adapter, m_firstPoint, hit));
        }
        removePreview();
        m_firstPoint = nullptr;
        showStatus(QObject::tr("Ersten Punkt klicken"));
    }
    event->accept();
}

void CreateMidpointTool::mouseMoveEvent(QMouseEvent* event) {
    if (!m_firstPoint) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    setPreviewLine(QLineF(m_firstPoint->x(), m_firstPoint->y(),
                          scenePos.x(),      scenePos.y()));
    event->accept();
}