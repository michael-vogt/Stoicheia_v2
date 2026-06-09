#include "CreateIntersectionTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreateIntersectionCommand.h"

CreateIntersectionTool::CreateIntersectionTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreateIntersectionTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());

    // Nur LinearObject oder Circle akzeptieren
    GeoObject* hit = nullptr;
    if (auto* linearObject = m_ctx.hitTest->linearObjectAt(scenePos)) {
        hit = linearObject;
    } else if (auto* circle = m_ctx.hitTest->circleAt(scenePos)) {
        hit = circle;
    }

    if (hit == nullptr) {
        event->accept(); return; 
    }

    if (m_firstObject == nullptr) {
        m_firstObject = hit;
        highlightObject(m_firstObject, true);
        showStatus(tr("Zweites Objekt klicken (Gerade oder Kreis)"));
    } else {
        if (hit != m_firstObject) {
            m_ctx.commandStack->execute(
                std::make_unique<CreateIntersectionCommand>(
                    m_ctx.adapter, m_firstObject, hit));
        }
        removePreview();
        m_firstObject = nullptr;
        showStatus(tr("Erstes Objekt klicken (Gerade oder Kreis)"));
    }
    event->accept();
}