#include "SelectTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/MoveCenterCommand.h"
#include <ui/commands/MacroCommand.h>
#include <ui/commands/DeleteObjectCommand.h>

#include "ui/commands/MergePointsCommand.h"

SelectTool::SelectTool(const ToolContext &ctx) : Tool(ctx) {}

void SelectTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->showStatusLeft(QObject::tr("Objekt auswählen und/oder verschieben"));
}

void SelectTool::deactivate() {
    setMergeCandidate(nullptr);
    if (m_rubberBand) {
        m_rubberBand->hide();
        delete m_rubberBand;
        m_rubberBand = nullptr;
    }
    m_rubberBanding = false;
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

Point* SelectTool::nearbyPoint(const QPointF &scenePos, Point *exclude) const {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(10, 10), QSize(20, 20)));
    for (QGraphicsItem *item : items) {
        if (auto* pi = dynamic_cast<GeoPointItem*>(item)) {
            Point* p = pi->point();
            if (p != exclude && p->isValid() && pi->isVisible())
                return p;
        }
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

void SelectTool::setMergeCandidate(Point *candidate) {
    if (m_mergeCandidate == candidate) return;
    if (m_mergeCandidate)
        m_ctx.adapter->highlight(m_mergeCandidate, false);
    m_mergeCandidate = candidate;
    if (m_mergeCandidate)
        m_ctx.adapter->highlight(m_mergeCandidate, true);
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
        m_ctx.adapter->select(m_draggedPoint);

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
            if (!(event->modifiers() & Qt::ControlModifier))
                m_ctx.adapter->clearSelection();
            startRubberBand(event->pos());
            //m_ctx.adapter->clearSelection();
            event->accept();
        }
    }
}

void SelectTool::mouseMoveEvent(QMouseEvent *event) {
    if (m_rubberBanding) {
        updateRubberBand(event->pos());
        event->accept();
        return;
    }
    if (!m_draggedPoint || !m_activeMove) {
        event->ignore();
        return;
    }

    const bool snapActive = event->modifiers() & Qt::AltModifier;

    const QPointF newPos = m_ctx.snapHelper->snap(m_ctx.drawingBoard->mapToScene(event->pos()) - m_dragOffset, snapActive);
    m_activeMove->setTarget(newPos.x(), newPos.y());
    m_activeMove->execute();

    // Merge-Kandidt aktualisieren
    Point* nearby = nearbyPoint(QPointF(m_draggedPoint->x(), m_draggedPoint->y()), m_draggedPoint);
    setMergeCandidate(nearby);

    event->accept();
}

void SelectTool::mouseReleaseEvent(QMouseEvent *event) {
    if (m_rubberBanding) {
        finishRubberBand(event->pos());
        event->accept();
        return;
    }
    if (!m_draggedPoint || !m_activeMove) {
        event->ignore();
        return;
    }

    // Merge-Kandidat zurücksetzen
    Point* mergeTarget = m_mergeCandidate;
    setMergeCandidate(nullptr);

    if (mergeTarget) {
        // Merge: gezogener Punkt wird in mergeTarget absorbiert
        auto macro = std::make_unique<MacroCommand>(QObject::tr("Punkte zusammenführen"));
        macro->add(std::move(m_activeMove));
        macro->add(std::make_unique<MergePointsCommand>(
            m_ctx.adapter, mergeTarget, m_draggedPoint));
        m_ctx.commandStack->execute(std::move(macro));
    } else {
        // Normaler Drag: Nur aufden Stack legen
        m_ctx.commandStack->pushWithoutExecute(std::move(m_activeMove));
    }

    m_draggedPoint = nullptr;
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    event->accept();
}

void SelectTool::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        const auto selection = m_ctx.adapter->selection();
        if (!selection.empty()) {
            std::vector<GeoObject*> toDelete;
            std::function<void(GeoObject*)> collect = [&](GeoObject* obj) {
                if (std::ranges::contains(toDelete, obj)) return;
                toDelete.push_back(obj);
                for (GeoObject* dep : obj->dependents())
                    collect(dep);
            };

            for (GeoObject* obj : selection)
                collect(obj);

            auto macro = std::make_unique<MacroCommand>(QObject::tr("Objekte löschen"));
            for (auto it = toDelete.rbegin(); it != toDelete.rend(); ++it)
                macro->add(std::make_unique<DeleteObjectCommand>(m_ctx.adapter, *it));

            m_ctx.adapter->clearSelection();
            m_ctx.commandStack->execute(std::move(macro));
            /*auto macro = std::make_unique<MacroCommand>(QObject::tr("Objekte löschen"));
            for (GeoObject* obj : selection) {
                macro->add(std::make_unique<DeleteObjectCommand>(m_ctx.adapter, obj));
            }
            m_ctx.adapter->clearSelection();
            m_ctx.commandStack->execute(std::move(macro));*/
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape && m_activeMove) {
        setMergeCandidate(nullptr);
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

void SelectTool::startRubberBand(const QPoint &viewPos) {
    m_rubberBanding = true;
    m_rubberStart = viewPos;
    if (!m_rubberBand)
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, m_ctx.drawingBoard->viewport());
    m_rubberBand->setGeometry(QRect(viewPos, QSize()));
    m_rubberBand->show();
}

void SelectTool::updateRubberBand(const QPoint& viewPos) {
    if (!m_rubberBand) return;
    m_rubberBand->setGeometry(
        QRect(m_rubberStart, viewPos).normalized());
}

void SelectTool::finishRubberBand(const QPoint& viewPos) {
    m_rubberBanding = false;
    if (!m_rubberBand) return;
    m_rubberBand->hide();

    // Rechteck in Szenenkoordinaten
    QRect  viewRect  = QRect(m_rubberStart, viewPos).normalized();
    QRectF sceneRect = m_ctx.drawingBoard->mapToScene(viewRect).boundingRect();

    // Alle Items im Rechteck selektieren
    const auto items = m_ctx.drawingBoard->scene()->items(
        sceneRect, Qt::ContainsItemBoundingRect);

    for (QGraphicsItem* item : items) {
        if (auto* geoItem = dynamic_cast<GeoGraphicsItem*>(item)) {
            if (geoItem->geoObject() && geoItem->geoObject()->isValid())
                m_ctx.adapter->select(geoItem->geoObject());
        }
    }
}
