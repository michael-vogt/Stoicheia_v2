#include "CreatePointTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreatePointCommand.h"


constexpr double DEFAULT_POINT_RADIUS = 4;
constexpr double DEFAULT_POINT_PENWIDTH = 1.5;

CreatePointTool::CreatePointTool(const ToolContext &ctx) : Tool(ctx) {}

void CreatePointTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->showStatusLeft(tr("Punkt durch Klicken hinzufügen"));
}

void CreatePointTool::deactivate() {
    if (m_preview != nullptr) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
    }
    m_ctx.drawingBoard->showStatusLeft("");
}

void CreatePointTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    bool snapActive = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    m_ctx.commandStack->execute(std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y()));
    event->accept();
}

void CreatePointTool::mouseMoveEvent(QMouseEvent *event) {
    bool snapActive = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    updatePreview(snapped);
    event->accept();
}

void CreatePointTool::updatePreview(const QPointF& scenePos) {
    if (m_preview == nullptr) {
        m_preview = new QGraphicsEllipseItem();
        m_preview->setPen(QPen(Qt::gray, DEFAULT_POINT_PENWIDTH));
        m_preview->setBrush(QBrush(Qt::white));
        // Preview nicht durch HitTest treffbar
        m_preview->setFlag(QGraphicsItem::ItemIsSelectable, false);
        m_ctx.drawingBoard->scene()->addItem(m_preview);
    }

    m_preview->setRect(scenePos.x() - DEFAULT_POINT_RADIUS,
                       scenePos.y() - DEFAULT_POINT_RADIUS,
                       DEFAULT_POINT_RADIUS * 2, 2 * DEFAULT_POINT_RADIUS);
}

void CreatePointTool::removePreview() {
    if (m_preview != nullptr) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
        delete m_preview;
        m_preview = nullptr;
    }
}