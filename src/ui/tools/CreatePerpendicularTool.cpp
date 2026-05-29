#include "CreatePerpendicularTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreatePerpendicularCommand.h"
#include <cmath>

CreatePerpendicularTool::CreatePerpendicularTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreatePerpendicularTool::onActivate() {
    showStatus(tr("Referenzgerade klicken"));
}

void CreatePerpendicularTool::onCancel() {
    m_reference = nullptr;
}

void CreatePerpendicularTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());

    if (!m_reference) {
        m_reference = m_ctx.hitTest->linearObjectAt(scenePos);
        if (m_reference) {
            highlightObject(m_reference, true);
            showStatus(tr("Punkt klicken durch den die Senkrechte läuft"));
        }
    } else {
        Point* origin = m_ctx.hitTest->pointAt(scenePos);
        if (origin) {
            m_ctx.commandStack->execute(
                std::make_unique<CreatePerpendicularCommand>(
                    m_ctx.adapter, origin, m_reference));
            removePreview();
            m_reference = nullptr;
            showStatus(tr("Referenzgerade klicken"));
        }
    }
    event->accept();
}

void CreatePerpendicularTool::mouseMoveEvent(QMouseEvent* event) {
    if (!m_reference) { event->ignore(); return; }

    // Vorschau: Senkrechte durch aktuelle Mausposition
    QPointF pos = m_ctx.drawingBoard->mapToScene(event->pos());
    double dx = -m_reference->dy(), dy = m_reference->dx(); // senkrecht
    const double ext = 10000.0;
    double len = std::sqrt(dx*dx + dy*dy);
    if (len > 1e-10) {
        dx /= len; dy /= len;
        setPreviewLine(QLineF(pos.x() - dx*ext, pos.y() - dy*ext,
                              pos.x() + dx*ext, pos.y() + dy*ext));
    }
    event->accept();
}