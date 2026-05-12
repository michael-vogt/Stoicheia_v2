#include "CreatePerpendicularFootTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreatePerpendicularFootCommand.h"

CreatePerpendicularFootTool::CreatePerpendicularFootTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreatePerpendicularFootTool::onActivate() {
    showStatus(QObject::tr("Punkt klicken"));
}

void CreatePerpendicularFootTool::onCancel() {
    m_point = nullptr;
}

void CreatePerpendicularFootTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());

    if (!m_point) {
        m_point = m_ctx.hitTest->pointAt(scenePos);
        if (m_point) {
            highlightObject(m_point, true);
            showStatus(QObject::tr("Gerade klicken"));
        }
    } else {
        LinearObject* line = m_ctx.hitTest->linearObjectAt(scenePos);
        if (line) {
            m_ctx.commandStack->execute(
                std::make_unique<CreatePerpendicularFootCommand>(
                    m_ctx.adapter, m_point, line));
            removePreview();
            m_point = nullptr;
            showStatus(QObject::tr("Punkt klicken"));
        }
    }
    event->accept();
}

void CreatePerpendicularFootTool::mouseMoveEvent(QMouseEvent* event) {
    event->ignore();
}