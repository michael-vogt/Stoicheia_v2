#include "CreatePerpendicularTool.h"
#include "../DrawingBoard.h"
#include "../commands/CreatePerpendicularCommand.h"
#include <cmath>
#include <limits>
#include "Constants.h"


constexpr double eps = std::numeric_limits<double>::epsilon();

CreatePerpendicularTool::CreatePerpendicularTool(const ToolContext& ctx)
    : ConstructionTool(ctx)
{}

void CreatePerpendicularTool::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());

    if (m_reference == nullptr) {
        m_reference = m_ctx.hitTest->linearObjectAt(scenePos);
        if (m_reference != nullptr) {
            highlightObject(m_reference, true);
            showStatus(tr("Punkt klicken durch den die Senkrechte läuft"));
        }
    } else {
        Point* origin = m_ctx.hitTest->pointAt(scenePos);
        if (origin != nullptr) {
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
    if (m_reference == nullptr) { event->ignore(); return; }

    // Vorschau: Senkrechte durch aktuelle Mausposition
    QPointF pos = m_ctx.drawingBoard->mapToScene(event->pos());
    double delta_x = -m_reference->dy();
    double delta_y = m_reference->dx(); // senkrecht
    double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));
    if (len > eps) {
        delta_x /= len;
        delta_y /= len;
        setPreviewLine(
            QLineF(pos.x() - (delta_x * Constants::Geometry::PERPENDICULAR_EXTENT),
                   pos.y() - (delta_y * Constants::Geometry::PERPENDICULAR_EXTENT),
                   pos.x() + (delta_x * Constants::Geometry::PERPENDICULAR_EXTENT),
                   pos.y() + (delta_y * Constants::Geometry::PERPENDICULAR_EXTENT)));
    }
    event->accept();
}