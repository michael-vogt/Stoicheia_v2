#include "SelectTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"

SelectTool::SelectTool(const ToolContext &ctx) : Tool(ctx) {}

void SelectTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
}

void SelectTool::deactivate() {
    m_draggedPoint = nullptr;
    m_activeMove = nullptr;
}

QCursor SelectTool::cursor() const {
    return m_draggedPoint ? Qt::ClosedHandCursor : Qt::ArrowCursor;
}

Point *SelectTool::pointAt(const QPointF &scenePos) const {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8, 8), QSize(16, 16)));
    for (QGraphicsItem *item : items) {
        if (auto* pointItem = dynamic_cast<GeoPointItem *>(item))
            return pointItem->point();
    }
    return nullptr;
}

void SelectTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    m_draggedPoint = pointAt(scenePos);

    if (m_draggedPoint) {
        m_dragOffset = scenePos - QPointF(m_draggedPoint->x(), m_draggedPoint->y());
        m_activeMove = std::make_unique<MovePointCommand>(m_draggedPoint, m_draggedPoint->x(), m_draggedPoint->y());
        m_ctx.drawingBoard->viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
}

void SelectTool::mouseMoveEvent(QMouseEvent *event) {
    if (!m_draggedPoint || !m_activeMove) {
        event->ignore();
        return;
    }

    bool snapActive = event->modifiers() & Qt::AltModifier;
    //QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    //QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    //QPointF newPos = scenePos - m_dragOffset;
    QPointF newPos = m_ctx.snapHelper->snap(m_ctx.drawingBoard->mapToScene(event->pos()) - m_dragOffset, snapActive);

    m_activeMove->setTarget(newPos.x(), newPos.y());
    m_draggedPoint->moveTo(newPos.x(), newPos.y());
    event->accept();
}

void SelectTool::mouseReleaseEvent(QMouseEvent *event) {
    if (!m_draggedPoint || !m_activeMove) {
        event->ignore();
        return;
    }

    m_ctx.commandStack->execute(std::move(m_activeMove));
    m_draggedPoint = nullptr;
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    event->accept();
}

void SelectTool::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        // Ausgewähles Objekt löschen, noch nicht implementiert
        event->accept();
        return;
    }
    event->ignore();
}
