#include "CreatePointTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreatePointCommand.h"

CreatePointTool::CreatePointTool(const ToolContext &ctx) : Tool(ctx) {}

void CreatePointTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->showStatus(QObject::tr("Punkt durch Klicken hinzufügen"));
}

void CreatePointTool::deactivate() {
    m_ctx.drawingBoard->showStatus("");
}

QCursor CreatePointTool::cursor() const {
    return Qt::CrossCursor;
}

void CreatePointTool::updatePreview(const QPointF& scenePos) {
    if (!m_preview) {
        m_preview = new QGraphicsEllipseItem();
        m_preview->setPen(QPen(Qt::gray, 1.5));
        m_preview->setBrush(QBrush(Qt::white));
        // Preview nicht durch HitTest treffbar
        m_preview->setFlag(QGraphicsItem::ItemIsSelectable, false);
        m_ctx.drawingBoard->scene()->addItem(m_preview);
    }

    m_preview->setRect(scenePos.x() - RADIUS, scenePos.y() - RADIUS, RADIUS * 2, RADIUS * 2);
}

void CreatePointTool::removePreview() {
    if (m_preview) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
        delete m_preview;
        m_preview = nullptr;
    }
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
    bool snapActive = event->modifiers() & Qt::AltModifier;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    updatePreview(snapped);
    event->accept();
}
