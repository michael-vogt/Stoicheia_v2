#include "SelectTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/MoveCenterCommand.h"
#include <ui/commands/MacroCommand.h>
#include <ui/commands/DeleteObjectCommand.h>

#include "ui/commands/CopyCommand.h"
#include "ui/commands/MergePointsCommand.h"
#include "geometry/UpdateGuard.h"

SelectTool::SelectTool(const ToolContext &ctx)
: Tool(ctx)
{}

void SelectTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->showStatusLeft(tr("Objekt auswählen und/oder verschieben"));
}

void SelectTool::deactivate() {
    setMergeCandidate(nullptr);
    if (m_rubberBand) {
        m_rubberBand->hide();
        delete m_rubberBand;
        m_rubberBand = nullptr;
    }
    m_rubberBanding = false;
}

void SelectTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    Point* hit = pointAt(scenePos);

    bool doMultiDrag = (hit) ? m_ctx.adapter->selection().contains(hit) : event->modifiers() & Qt::ShiftModifier;
    if (doMultiDrag) {
        // Punkt ist in der Selektion -> alle selektierten Punkte mitbewegen
        startMultiDrag(scenePos);
        event->accept();
        return;
    }

    if (hit) {
        // Punkt außerhalb Selektion -> nur diesen selektieren und bewegen
        if (!(event->modifiers() & Qt::ControlModifier))
            m_ctx.adapter->clearSelection();
        m_ctx.adapter->select(hit);
        startMultiDrag(scenePos);
        event->accept();
        return;
    }

    // Kein Punkt - anderes Objekt oder Rubberband
    GeoObject* obj = m_ctx.hitTest->anyObjectAt(scenePos);
    if (obj) {
        if (!(event->modifiers() & Qt::ControlModifier))
            m_ctx.adapter->clearSelection();
        m_ctx.adapter->select(obj);
    } else {
        if (!(event->modifiers() & Qt::ControlModifier))
            m_ctx.adapter->clearSelection();
        startRubberBand(event->pos());
    }
    event->accept();
}

void SelectTool::mouseMoveEvent(QMouseEvent *event) {
    if (m_rubberBanding) {
        updateRubberBand(event->pos());
        event->accept();
        return;
    }

    if (m_activeMoves.empty()) {
        event->ignore();
        return;
    }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF delta = scenePos - m_dragOffset - QPointF(m_draggedPoints.front()->x(), m_draggedPoints.front()->y());

    // Alle Punkte um denselben Delta verschieben
    {
        UpdateGuard guard;
        for (size_t i = 0; i < m_draggedPoints.size(); ++i) {
            double newX = m_draggedPoints[i]->x() + delta.x();
            double newY = m_draggedPoints[i]->y() + delta.y();
            m_activeMoves[i]->setTarget(newX, newY);
            m_activeMoves[i]->execute();
        }
    }

    // Merge-Kandidat nur bei Einzelpunkt-Drag
    if (m_draggedPoints.size() == 1) {
        Point* nearby = nearbyPoint(QPointF(m_draggedPoints[0]->x(), m_draggedPoints[0]->y()), m_draggedPoints[0]);
        setMergeCandidate(nearby);
    }

    event->accept();
}

void SelectTool::mouseReleaseEvent(QMouseEvent *event) {
    if (m_rubberBanding) {
        finishRubberBand(event->pos());
        event->accept();
        return;
    }

    if (m_activeMoves.empty()) {
        event->ignore();
        return;
    }

    Point* mergeTarget = m_mergeCandidate;
    setMergeCandidate(nullptr);

    if (mergeTarget && m_draggedPoints.size() == 1) {
        // Merge nur bei Einzelpunkt-Drag
        auto macro = std::make_unique<MacroCommand>(tr("Punkte zusammenführen"));
        for (auto& move : m_activeMoves)
            macro->add(std::move(move));
        macro->add(std::make_unique<MergePointsCommand>(m_ctx.adapter, mergeTarget, m_draggedPoints[0]));
        m_ctx.commandStack->execute(std::move(macro));
    } else if (m_activeMoves.size() == 1) {
        m_ctx.commandStack->pushWithoutExecute(std::move(m_activeMoves[0]));
    } else {
        // Mehrere Punkte -> Macro
        auto macro = std::make_unique<MacroCommand>(tr("Objekte verschieben"));
        for (auto& move : m_activeMoves)
            macro->add(std::move(move));
        m_ctx.commandStack->pushWithoutExecute(std::move(macro));
    }

    m_activeMoves.clear();
    m_draggedPoints.clear();
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    event->accept();
}

void SelectTool::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        const auto& sel = m_ctx.adapter->selection();
        m_ctx.adapter->copySelection();
        event->accept();
        return;
    }

    if (event->matches(QKeySequence::Paste)) {
        const auto& clipboard = m_ctx.adapter->clipboard();
        if (!clipboard.empty()) {
            m_ctx.commandStack->execute(
                std::make_unique<CopyCommand>(
                    m_ctx.adapter,
                    clipboard,
                    QPointF(50, -50)));
        }
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        const auto selection = m_ctx.adapter->selection();
        if (!selection.empty()) {
            std::vector<GeoObject*> toDelete;
            std::vector<GeoObject*> toDeleteUpward;
            std::function<void(GeoObject*)> collect = [&](GeoObject* obj) {
                if (std::ranges::contains(toDelete, obj)) return;
                if (dynamic_cast<QGraphicsItem*>(obj)) return;
                for (GeoObject* dep : obj->dependents())
                    collect(dep);
                toDelete.push_back(obj);
            };

            std::function<void(GeoObject*)> collectUpward = [&](GeoObject* obj) {
                if (std::ranges::contains(toDeleteUpward, obj)) return;
                if (dynamic_cast<QGraphicsItem*>(obj)) return;

                std::vector<GeoObject*> geoObjectDeps;
                for (GeoObject* dep : obj->dependents()) {
                    if (dynamic_cast<QGraphicsItem*>(dep)) continue;
                    geoObjectDeps.push_back(dep);
                }

                if (!geoObjectDeps.empty()) {
                    for (GeoObject* dep : geoObjectDeps) {
                        collectUpward(dep);
                        toDeleteUpward.push_back(obj);
                    }
                } else {
                    if (!dynamic_cast<QGraphicsItem*>(obj))
                        toDeleteUpward.push_back(obj);
                }
            };

            for (GeoObject* obj : selection) {
                collect(obj);
                collectUpward(obj);
            }

            auto macro = std::make_unique<MacroCommand>(tr("Objekte löschen"));
            for (GeoObject* obj : toDeleteUpward)
                macro->add(std::make_unique<DeleteObjectCommand>(m_ctx.adapter, obj));

            m_ctx.adapter->clearSelection();
            m_ctx.commandStack->execute(std::move(macro));
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape && !m_activeMoves.empty()) {
        setMergeCandidate(nullptr);
        for (auto& move : m_activeMoves)
            move->undo();
        m_activeMoves.clear();
        m_draggedPoints.clear();
        m_ctx.drawingBoard->viewport()->setCursor(cursor());
        event->accept();
        return;
    }
    event->ignore();
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

void SelectTool::startMultiDrag(const QPointF &scenePos) {
    m_draggedPoints.clear();
    m_activeMoves.clear();

    const auto& sel = m_ctx.adapter->selection();

    // Alle draggbaren Punkte aus der Selektion sammeln
    for (GeoObject* obj : sel) {
        auto* p = dynamic_cast<Point*>(obj);
        if (!p) continue;

        GeoGraphicsItem* item = m_ctx.adapter->itemFor(p);
        auto* pi = dynamic_cast<GeoPointItem*>(item);
        if (!pi || !isDraggable(pi->point())) continue;

        m_draggedPoints.push_back(p);

        Point* radiusPoint = m_ctx.adapter->radiusPointFor(p);
        if (radiusPoint && !sel.contains(radiusPoint)) {
            // Kreismittelpunkt mitbewegen
            m_activeMoves.push_back(std::make_unique<MoveCenterCommand>(p, radiusPoint, p->x(), p->y()));
        } else if (!radiusPoint) {
            m_activeMoves.push_back(std::make_unique<MovePointCommand>(p, p->x(), p->y()));
        }
    }

    // Drag-Offset: relativ zum ersten Punkt
    if (!m_draggedPoints.empty()) {
        Point* first = m_draggedPoints.front();
        m_dragOffset = scenePos - QPointF(first->x(), first->y());
    }

    if (!m_draggedPoints.empty())
        m_ctx.drawingBoard->viewport()->setCursor(Qt::ClosedHandCursor);
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

void SelectTool::setMergeCandidate(Point *candidate) {
    if (m_mergeCandidate == candidate) return;
    if (m_mergeCandidate)
        m_ctx.adapter->highlight(m_mergeCandidate, false);
    m_mergeCandidate = candidate;
    if (m_mergeCandidate)
        m_ctx.adapter->highlight(m_mergeCandidate, true);
}
