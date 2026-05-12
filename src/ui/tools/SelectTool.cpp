#include "SelectTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/MoveCenterCommand.h"

SelectTool::SelectTool(const ToolContext &ctx) : Tool(ctx) {}

void SelectTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->showStatus(QObject::tr("Objekt auswählen und/oder verschieben"));
}

void SelectTool::deactivate() {
    m_ctx.adapter->clearSelection();
    m_ctx.drawingBoard->showStatus("");
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

GeoGraphicsItem* SelectTool::itemAt(const QPointF &scenePos, const std::type_info& type) const {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8, 8), QSize(16, 16)));
    for (QGraphicsItem *item : items) {
        if (auto* gi = dynamic_cast<GeoGraphicsItem*>(item)) {
            if (typeid(*gi) == type && gi->contains(scenePos))
                return gi;
        }
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

    if (m_draggedPoint && isDraggable(m_draggedPoint)) {
        if (!(event->modifiers() & Qt::ControlModifier))
            m_ctx.adapter->clearSelection();
        m_ctx.adapter->select(static_cast<GeoObject*>(m_draggedPoint));

        if (Point* radiusPoint = m_ctx.adapter->radiusPointFor(m_draggedPoint)) {
            m_activeMove = std::make_unique<MoveCenterCommand>(m_draggedPoint, radiusPoint, m_draggedPoint->x(), m_draggedPoint->y());
        } else {
            m_activeMove = std::make_unique<MovePointCommand>(m_draggedPoint, m_draggedPoint->x(), m_draggedPoint->y());
        }
        m_dragOffset = scenePos - QPointF(m_draggedPoint->x(), m_draggedPoint->y());
        m_ctx.drawingBoard->viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        GeoGraphicsItem* hit = itemAt(scenePos, typeid(GeoLinearObjectItem));
        if (!hit)
            hit = itemAt(scenePos, typeid(GeoCircleItem));

        if (hit) {
            if (!(event->modifiers() & Qt::ControlModifier))
                m_ctx.adapter->clearSelection();
            m_ctx.adapter->select(hit->geoObject());
            hit->setGeoSelected(true);
            event->accept();
        } else {
            m_ctx.adapter->clearSelection();
            event->accept();
        }
    }
}

void SelectTool::mouseMoveEvent(QMouseEvent *event) {
    if (!m_draggedPoint || !m_activeMove) {
        event->ignore();
        return;
    }

    const bool snapActive = event->modifiers() & Qt::AltModifier;

    const QPointF newPos = m_ctx.snapHelper->snap(m_ctx.drawingBoard->mapToScene(event->pos()) - m_dragOffset, snapActive);
    m_activeMove->setTarget(newPos.x(), newPos.y());
    m_activeMove->execute();
    event->accept();
}

void SelectTool::mouseReleaseEvent(QMouseEvent *event) {
    if (!m_draggedPoint || !m_activeMove) {
        event->ignore();
        return;
    }

    m_ctx.commandStack->pushWithoutExecute(std::move(m_activeMove));
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
    if (event->key() == Qt::Key_Escape && m_activeMove) {
        m_activeMove->undo();
        m_activeMove = nullptr;
        m_draggedPoint = nullptr;
        m_ctx.drawingBoard->viewport()->setCursor(cursor());
        event->accept();
        return;
    }
    event->ignore();
}

bool SelectTool::isDraggable(Point* point) const {
    return typeid(*point) == typeid(Point);
}
