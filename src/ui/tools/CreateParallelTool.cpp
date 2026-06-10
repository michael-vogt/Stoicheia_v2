#include "CreateParallelTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreateParallelCommand.h"
#include "../../Constants.h"


using namespace Constants;

CreateParallelTool::CreateParallelTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreateParallelTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());

    if (m_reference == nullptr) {
        m_reference = m_ctx.hitTest->linearObjectAt(scenePos);
        if (m_reference != nullptr) {
            highlightObject(m_reference, true);
            showStatus(tr("Punkt klicken durch den die Parallele läuft"));
        }
    } else {
        Point* origin = m_ctx.hitTest->pointAt(scenePos);
        if (origin != nullptr) {
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
    if (m_reference == nullptr) {
        event->ignore();
        return; 
    }

    // Vorschau: Parallele durch aktuelle Mausposition
    QPointF pos = m_ctx.drawingBoard->mapToScene(event->pos());
    double delta_x = m_reference->dx();
    double delta_y = m_reference->dy();
    double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));

    if (len > NumericConstants::DOUBLE_EPS) {
        delta_x /= len;
        delta_y /= len;
        setPreviewLine(QLineF(pos.x() - (delta_x*GeometryConstants::PARALLEL_EXTENT), pos.y() - (delta_y*GeometryConstants::PARALLEL_EXTENT),
                              pos.x() + (delta_x*GeometryConstants::PARALLEL_EXTENT), pos.y() + (delta_y*GeometryConstants::PARALLEL_EXTENT)));
    }
    event->accept();
}