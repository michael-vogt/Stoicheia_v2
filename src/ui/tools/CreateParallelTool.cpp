#include "CreateParallelTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreateParallelCommand.h"

CreateParallelTool::CreateParallelTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreateParallelTool::onActivate() {
    showStatus(tr("Referenzgerade klicken"));
}

void CreateParallelTool::onCancel() {
    m_reference = nullptr;
}

void CreateParallelTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());

    if (!m_reference) {
        m_reference = m_ctx.hitTest->linearObjectAt(scenePos);
        if (m_reference) {
            highlightObject(m_reference, true);
            showStatus(tr("Punkt klicken durch den die Parallele läuft"));
        }
    } else {
        Point* origin = m_ctx.hitTest->pointAt(scenePos);
        if (origin) {
            m_ctx.commandStack->execute(
                std::make_unique<CreateParallelCommand>(
                    m_ctx.adapter, origin, m_reference));
            removePreview();
            m_reference = nullptr;
            showStatus(tr("Referenzgerade klicken"));
        }
    }
    event->accept();
}

void CreateParallelTool::mouseMoveEvent(QMouseEvent* event) {
    if (!m_reference) { event->ignore(); return; }

    // Vorschau: Parallele durch aktuelle Mausposition
    QPointF pos = m_ctx.drawingBoard->mapToScene(event->pos());
    double dx = m_reference->dx(), dy = m_reference->dy();
    const double ext = 10000.0;
    double len = std::sqrt(dx*dx + dy*dy);
    if (len > 1e-10) {
        dx /= len; dy /= len;
        setPreviewLine(QLineF(pos.x() - dx*ext, pos.y() - dy*ext,
                              pos.x() + dx*ext, pos.y() + dy*ext));
    }
    event->accept();
}