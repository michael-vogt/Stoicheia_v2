#include "SelectTool.h"

#include "CoordinateInputDialog.h"
#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/MoveCenterCommand.h"
#include <memory>
#include <QColorDialog>
#include <qevent.h>
#include <QMenu>
#include <qnamespace.h>
#include <ui/commands/MacroCommand.h>
#include <ui/commands/DeleteObjectCommand.h>

#include "ui/commands/CopyCommand.h"
#include "ui/commands/MergePointsCommand.h"
#include "geometry/UpdateGuard.h"
#include "ui/commands/MovePointCommand.h"
#include "../../Constants.h"
#include "geometry/Line.h"
#include "ui/commands/ConstrainPointCommand.h"
#include "ui/commands/SetColorCommand.h"
#include "ui/commands/SetVisibilityCommand.h"


using namespace Constants;

SelectTool::SelectTool(const ToolContext &ctx)
: Tool(ctx)
{}

void SelectTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->showStatusLeft(tr("Objekt auswählen und/oder verschieben"));
}

void SelectTool::deactivate() {
    setMergeCandidate(nullptr);
    setConstrainCandidate(nullptr);
    if (m_rubberBand != nullptr) {
        m_rubberBand->hide();
        delete m_rubberBand;
        m_rubberBand = nullptr;
    }
    m_rubberBanding = false;
}

void SelectTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        const QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
        GeoObject* obj = m_ctx.hitTest->anyObjectAt(scenePos);
        if (obj == nullptr) {
            Point* p = pointAt(scenePos);
            obj = p;
        }
        if (obj != nullptr) {
            showContextMenu(event, obj);
            event->accept();
            return;
        }
        event->ignore();
        return;
    }

    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    const QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    Point* hit = pointAt(scenePos);

    bool doMultiDrag = ((hit) != nullptr) ? m_ctx.adapter->selection().contains(hit) : (event->modifiers() & Qt::ShiftModifier) != 0;
    if (doMultiDrag) {
        // Punkt ist in der Selektion -> alle selektierten Punkte mitbewegen
        startMultiDrag(scenePos);
        event->accept();
        return;
    }

    if (hit != nullptr) {
        // Punkt außerhalb Selektion -> nur diesen selektieren und bewegen
        if (!(event->modifiers() & Qt::ControlModifier)) {
            m_ctx.adapter->clearSelection();
        }
        m_ctx.adapter->select(hit);
        startMultiDrag(scenePos);
        event->accept();
        return;
    }

    // Kein Punkt - anderes Objekt oder Rubberband
    GeoObject* obj = m_ctx.hitTest->anyObjectAt(scenePos);
    if (obj != nullptr) {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            m_ctx.adapter->clearSelection();
        }
        m_ctx.adapter->select(obj);
    } else {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            m_ctx.adapter->clearSelection();
        }
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

    const QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    const QPointF delta = scenePos - m_dragOffset - QPointF(m_draggedPoints.front()->x(), m_draggedPoints.front()->y());

    // Alle Punkte um denselben Delta verschieben
    {
        UpdateGuard guard;
        for (size_t i = 0; i < m_draggedPoints.size(); ++i) {
            const double newX = m_draggedPoints[i]->x() + delta.x();
            const double newY = m_draggedPoints[i]->y() + delta.y();
            m_activeMoves[i]->setTarget(newX, newY);
            m_activeMoves[i]->execute();
        }
    }

    // Merge- oder Constrain-Kandidat nur bei Einzelpunkt-Drag
    if (m_draggedPoints.size() == 1) {
        Point* nearby = nearbyPoint(QPointF(m_draggedPoints[0]->x(), m_draggedPoints[0]->y()), m_draggedPoints[0]);
        setMergeCandidate(nearby);

        // Nur Constrain-Kandidat setzen, wenn kein Merge-Kandidat vorhanden
        if (nearby == nullptr) {
            GeoObject* constrainTarget = nullptr;
            QPointF pos(m_draggedPoints[0]->x(), m_draggedPoints[0]->y());
            constrainTarget = m_ctx.hitTest->linearObjectAt(pos);
            if (constrainTarget == nullptr) {
                constrainTarget = m_ctx.hitTest->circleAt(pos);
            }

            // nur Constrain-Kandidat setzen, wenn gezogener Punkt keine source des constrainTarget ist
            bool isDefiningPoint = false;
            if (auto* line = dynamic_cast<LinearObject*>(constrainTarget)) {

                for (GeoObject* obj : line->sources()) {
                    if (obj == m_draggedPoints[0]) {
                        isDefiningPoint = true;
                        break;
                    }
                }
            } else if (auto* circle = dynamic_cast<Circle*>(constrainTarget)) {
                for (GeoObject* obj : circle->sources()) {
                    if (obj == m_draggedPoints[0]) {
                        isDefiningPoint = true;
                        break;
                    }
                }
            }

            if (!isDefiningPoint) {
                setConstrainCandidate(constrainTarget);
            }
        } else {
            setConstrainCandidate(nullptr);
        }
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
    GeoObject* constrainTarget = m_constrainCandidate;
    setMergeCandidate(nullptr);
    setConstrainCandidate(nullptr);

    if ((mergeTarget != nullptr) && m_draggedPoints.size() == 1) {
        // Merge nur bei Einzelpunkt-Drag
        auto macro = std::make_unique<MacroCommand>(tr("Punkte zusammenführen"));
        for (auto& move : m_activeMoves) {
            macro->add(std::move(move));
        }
        macro->add(std::make_unique<MergePointsCommand>(m_ctx.adapter, mergeTarget, m_draggedPoints[0]));
        m_ctx.commandStack->execute(std::move(macro));
    } else if (constrainTarget != nullptr && m_draggedPoints.size() == 1) {
        // Punkt einschränken
        auto macro = std::make_unique<MacroCommand>(tr("Punkt einschränken"));
        for (auto& move : m_activeMoves) {
            macro->add(std::move(move));
        }
        if (auto* line = dynamic_cast<LinearObject*>(constrainTarget)) {
            macro->add(std::make_unique<ConstrainPointToLineCommand>(m_ctx.adapter, m_draggedPoints[0], line));
        } else if (auto* circle = dynamic_cast<Circle*>(constrainTarget)) {
            macro->add(std::make_unique<ConstrainPointToCircleCommand>(m_ctx.adapter, m_draggedPoints[0], circle));
        }
        m_ctx.commandStack->execute(std::move(macro));
        m_constrainCandidate = nullptr;
    } else if (m_activeMoves.size() == 1) {
        m_ctx.commandStack->pushWithoutExecute(std::move(m_activeMoves[0]));
    } else {
        // Mehrere Punkte -> Macro
        auto macro = std::make_unique<MacroCommand>(tr("Objekte verschieben"));
        for (auto& move : m_activeMoves) {
            macro->add(std::move(move));
        }
        m_ctx.commandStack->pushWithoutExecute(std::move(macro));
    }

    m_activeMoves.clear();
    m_draggedPoints.clear();
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    event->accept();
}

void SelectTool::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    Point* hit = pointAt(scenePos);

    if (hit == nullptr || !isDraggable(hit)) {
        event->ignore();
        return;
    }

    // Qt sendet bei Doppelklick: press→release→press→doubleClick.
    // Der erste release hat bereits einen MoveCommand auf den Stack gelegt.
    // Den nehmen wir zurück, bevor der Dialog geöffnet wird.
    m_ctx.commandStack->undoLast();

    CoordinateInputDialog dlg(m_ctx.drawingBoard);
    dlg.setCoordinates(hit->x(), hit->y());
    if (dlg.exec() == QDialog::Accepted) {
        QPointF pos = dlg.coordinates();
        m_ctx.commandStack->execute(
            std::make_unique<MovePointCommand>(hit, pos.x(), pos.y()));
    }
    event->accept();
}

void SelectTool::keyPressEvent(QKeyEvent *event) { // NOLINT
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
                    QPointF(UiMetricsConstants::PASTE_OFFSET, -UiMetricsConstants::PASTE_OFFSET)));
        }
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        const auto selection = m_ctx.adapter->selection();
        if (!selection.empty()) {
            std::vector<GeoObject*> toDelete;
            std::vector<GeoObject*> toDeleteUpward;
            std::function<void(GeoObject*)> collect = [&](GeoObject* obj) -> void {
                if (std::ranges::contains(toDelete, obj)) { 
                    return;
                }
                if (dynamic_cast<QGraphicsItem*>(obj)) {
                    return;
                }
                for (GeoObject* dep : obj->dependents()) {
                    collect(dep);
                }
                toDelete.push_back(obj);
            };

            std::function<void(GeoObject*)> collectUpward = [&](GeoObject* obj) -> void {
                if (std::ranges::contains(toDeleteUpward, obj)) {
                    return;
                }
                if (dynamic_cast<QGraphicsItem*>(obj)) {
                    return;
                }

                std::vector<GeoObject*> geoObjectDeps;
                for (GeoObject* dep : obj->dependents()) {
                    if (dynamic_cast<QGraphicsItem*>(dep)) {
                        continue;
                    }
                    geoObjectDeps.push_back(dep);
                }

                if (!geoObjectDeps.empty()) {
                    for (GeoObject* dep : geoObjectDeps) {
                        collectUpward(dep);
                        toDeleteUpward.push_back(obj);
                    }
                } else {
                    if (!dynamic_cast<QGraphicsItem*>(obj)) {
                        toDeleteUpward.push_back(obj);
                    }
                }
            };

            for (GeoObject* obj : selection) {
                collect(obj);
                collectUpward(obj);
            }

            auto macro = std::make_unique<MacroCommand>(tr("Objekte löschen"));
            for (GeoObject* obj : toDeleteUpward) {
                macro->add(std::make_unique<DeleteObjectCommand>(m_ctx.adapter, obj));
            }

            m_ctx.adapter->clearSelection();
            m_ctx.commandStack->execute(std::move(macro));
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape && !m_activeMoves.empty()) {
        setMergeCandidate(nullptr);
        for (auto& move : m_activeMoves) {
            move->undo();
        }
        m_activeMoves.clear();
        m_draggedPoints.clear();
        m_ctx.drawingBoard->viewport()->setCursor(cursor());
        event->accept();
        return;
    }
    event->ignore();
}

void SelectTool::showContextMenu(QMouseEvent *event, GeoObject *obj) {
    auto* item = m_ctx.adapter->itemFor(obj);
    if (item == nullptr) {
        return;
    }

    QMenu menu(m_ctx.drawingBoard);

    // Sichtbarkeit umschalten
    bool currentlyvisible = item->isVisible();
    QAction* visAction = menu.addAction(currentlyvisible ? tr("Ausblenden") : tr("Einblenden"));

    menu.addSeparator();

    // Farbe ändern
    QAction* colorAction = menu.addAction(tr("Farbe ändern..."));

    QAction* chosen = menu.exec(event->globalPosition().toPoint());

    if (chosen == visAction) {
        m_ctx.commandStack->execute(std::make_unique<SetVisibilityCommand>(m_ctx.adapter, obj, !currentlyvisible));
    } else if (chosen == colorAction) {
        // Aktuelle Farbe ermitteln
        QColor current = Qt::black;
        if (auto* pi = dynamic_cast<GeoPointItem*>(item)) {
            current = pi->pen().color();
        } else if (auto* li = dynamic_cast<GeoLinearObjectItem*>(item)) {
            current = li->pen().color();
        } else if (auto* ci = dynamic_cast<GeoCircleItem*>(item)) {
            current = ci->pen().color();
        }

        QColor newColor = QColorDialog::getColor(current, m_ctx.drawingBoard, tr("Farbe wählen"));
        if (newColor.isValid()) {
            m_ctx.commandStack->execute(std::make_unique<SetColorCommand>(m_ctx.adapter, obj, newColor));
        }

    }
}

auto SelectTool::pointAt(const QPointF &scenePos) const -> Point * {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8, 8), QSize(16, 16)));
    for (QGraphicsItem *item : items) {
        if (auto* pointItem = dynamic_cast<GeoPointItem *>(item)) {
            return pointItem->point();
        }
    }
    return nullptr;
}

auto SelectTool::nearbyPoint(const QPointF &scenePos, Point *exclude) const -> Point* {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(10, 10), QSize(20, 20)));
    for (QGraphicsItem *item : items) {
        if (auto* pointItem = dynamic_cast<GeoPointItem*>(item)) {
            Point* point = pointItem->point();
            if (point != exclude && point->isValid() && pointItem->isVisible()) {
                return point;
            }
        }
    }
    return nullptr;
}

auto SelectTool::itemAt(const QPointF &scenePos, const std::type_info& type) const -> GeoGraphicsItem* {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8, 8), QSize(16, 16)));
    for (QGraphicsItem *item : items) {
        if (auto* geoItem = dynamic_cast<GeoGraphicsItem*>(item)) {
            if (typeid(*geoItem) == type && geoItem->contains(scenePos)) {
                return geoItem;
            }
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
        auto* point = dynamic_cast<Point*>(obj);
        if (point == nullptr) {
            continue;
        }

        GeoGraphicsItem* item = m_ctx.adapter->itemFor(point);
        auto* pointItem = dynamic_cast<GeoPointItem*>(item);
        if ((pointItem == nullptr) || !isDraggable(pointItem->point())) {
            continue;
        }

        m_draggedPoints.push_back(point);

        Point* radiusPoint = m_ctx.adapter->radiusPointFor(point);
        if ((radiusPoint != nullptr) && !sel.contains(radiusPoint)) {
            // Kreismittelpunkt mitbewegen
            m_activeMoves.push_back(std::make_unique<MoveCenterCommand>(point, radiusPoint, point->x(), point->y()));
        } else if (radiusPoint == nullptr) {
            m_activeMoves.push_back(std::make_unique<MovePointCommand>(point, point->x(), point->y()));
        }
    }

    // Drag-Offset: relativ zum ersten Punkt
    if (!m_draggedPoints.empty()) {
        Point* first = m_draggedPoints.front();
        m_dragOffset = scenePos - QPointF(first->x(), first->y());
    }

    if (!m_draggedPoints.empty()) {
        m_ctx.drawingBoard->viewport()->setCursor(Qt::ClosedHandCursor);
    }
}

void SelectTool::startRubberBand(const QPoint &viewPos) {
    m_rubberBanding = true;
    m_rubberStart = viewPos;
    if (m_rubberBand == nullptr) {
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, m_ctx.drawingBoard->viewport());
    }
    m_rubberBand->setGeometry(QRect(viewPos, QSize()));
    m_rubberBand->show();
}

void SelectTool::updateRubberBand(const QPoint& viewPos) {
    if (m_rubberBand == nullptr) {
        return;
    }
    m_rubberBand->setGeometry(
        QRect(m_rubberStart, viewPos).normalized());
}

void SelectTool::finishRubberBand(const QPoint& viewPos) {
    m_rubberBanding = false;
    if (m_rubberBand == nullptr) {
        return;
    }
    m_rubberBand->hide();

    // Rechteck in Szenenkoordinaten
    QRect  viewRect  = QRect(m_rubberStart, viewPos).normalized();
    QRectF sceneRect = m_ctx.drawingBoard->mapToScene(viewRect).boundingRect();

    // Alle Items im Rechteck selektieren
    const auto items = m_ctx.drawingBoard->scene()->items(
        sceneRect, Qt::ContainsItemBoundingRect);

    for (QGraphicsItem* item : items) {
        if (auto* geoItem = dynamic_cast<GeoGraphicsItem*>(item)) {
            if ((geoItem->geoObject() != nullptr) && geoItem->geoObject()->isValid()) {
                m_ctx.adapter->select(geoItem->geoObject());
            }
        }
    }
}

void SelectTool::setMergeCandidate(Point *candidate) {
    if (m_mergeCandidate == candidate) {
        return;
    }
    if (m_mergeCandidate != nullptr) {
        m_ctx.adapter->highlight(m_mergeCandidate, false);
    }
    m_mergeCandidate = candidate;
    if (m_mergeCandidate != nullptr) {
        m_ctx.adapter->highlight(m_mergeCandidate, true);
    }
}

void SelectTool::setConstrainCandidate(GeoObject *candidate) {
    if (m_constrainCandidate == candidate) {
        return;
    }

    if (m_constrainCandidate != nullptr) {
        m_ctx.adapter->highlight(m_constrainCandidate, false);
    }
    m_constrainCandidate = candidate;
    if (m_constrainCandidate != nullptr) {
        m_ctx.adapter->highlight(m_constrainCandidate, true);
    }
}
