#include "CreateIntersectionTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreateIntersectionCommand.h"

CreateIntersectionTool::CreateIntersectionTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreateIntersectionTool::onActivate() {
    showStatus(QObject::tr("Erstes Objekt klicken (Gerade oder Kreis)"));
}

void CreateIntersectionTool::onCancel() {
    m_firstObject = nullptr;
}

void CreateIntersectionTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());

    // Nur LinearObject oder Circle akzeptieren
    GeoObject* hit = nullptr;
    if (auto* l = m_ctx.hitTest->linearObjectAt(scenePos)) hit = l;
    else if (auto* c = m_ctx.hitTest->circleAt(scenePos))  hit = c;

    if (!hit) { event->accept(); return; }

    if (!m_firstObject) {
        m_firstObject = hit;
        highlightObject(m_firstObject, true);
        showStatus(QObject::tr("Zweites Objekt klicken (Gerade oder Kreis)"));
    } else {
        if (hit != m_firstObject) {
            m_ctx.commandStack->execute(
                std::make_unique<CreateIntersectionCommand>(
                    m_ctx.adapter, m_firstObject, hit));
        }
        removePreview();
        m_firstObject = nullptr;
        showStatus(QObject::tr("Erstes Objekt klicken (Gerade oder Kreis)"));
    }
    event->accept();
}

void CreateIntersectionTool::mouseMoveEvent(QMouseEvent* event) {
    event->ignore();
}