#include "SelectTool.h"

#include "CoordinateInputDialog.h"
#include "Structs.h"
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
        const QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
        GeoObject* obj = m_ctx.hitTest->anyObjectAt(scene_pos);
        if (obj == nullptr) {
            Point* point = pointAt(scene_pos);
            obj = point;
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

    const QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    Point* hit = pointAt(scene_pos);

    bool do_multi_drag = (nullptr != hit)
                             ? m_ctx.adapter->selection().contains(hit)
                             : (event->modifiers() & Qt::ShiftModifier) != 0;
    if (do_multi_drag) {
        // Punkt ist in der Selektion -> alle selektierten Punkte mitbewegen
        startMultiDrag(scene_pos);
        event->accept();
        return;
    }

    if (hit != nullptr) {
        // Punkt außerhalb Selektion -> nur diesen selektieren und bewegen
        if (!(event->modifiers() & Qt::ControlModifier)) {
            m_ctx.adapter->clearSelection();
        }
        m_ctx.adapter->select(hit);
        startMultiDrag(scene_pos);
        event->accept();
        return;
    }

    // Kein Punkt - anderes Objekt oder Rubberband
    GeoObject* obj = m_ctx.hitTest->anyObjectAt(scene_pos);
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

void SelectTool::mouseMoveEvent(QMouseEvent *event) { // NOLINT: readability-function-cognitive-complexity
    if (m_rubberBanding) {
        updateRubberBand(event->pos());
        event->accept();
        return;
    }

    if (m_activeMoves.empty()) {
        event->ignore();
        return;
    }

    const QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    const QPointF delta = scene_pos - m_dragOffset - QPointF(m_draggedPoints.front()->x(), m_draggedPoints.front()->y());

    // Alle Punkte um denselben Delta verschieben
    {
        UpdateGuard guard;
        for (size_t i = 0; i < m_draggedPoints.size(); ++i) {
            const double new_x = m_draggedPoints[i]->x() + delta.x();
            const double new_y = m_draggedPoints[i]->y() + delta.y();
            m_activeMoves[i]->setTarget(new_x, new_y);
            m_activeMoves[i]->execute();
        }
    }

    // Merge- oder Constrain-Kandidat nur bei Einzelpunkt-Drag
    if (m_draggedPoints.size() == 1) {
        Point* nearby = nearbyPoint(QPointF(m_draggedPoints[0]->x(), m_draggedPoints[0]->y()), m_draggedPoints[0]);
        setMergeCandidate(nearby);

        // Nur Constrain-Kandidat setzen, wenn kein Merge-Kandidat vorhanden
        if (nearby == nullptr) {
            GeoObject* constrain_target = nullptr;
            QPointF pos(m_draggedPoints[0]->x(), m_draggedPoints[0]->y());
            constrain_target = m_ctx.hitTest->linearObjectAt(pos);
            if (constrain_target == nullptr) {
                constrain_target = m_ctx.hitTest->circleAt(pos);
            }

            // nur Constrain-Kandidat setzen, wenn gezogener Punkt keine source des constrainTarget ist
            bool is_defining_point = false;
            if (auto* line = dynamic_cast<LinearObject*>(constrain_target)) {

                for (GeoObject* obj : line->sources()) {
                    if (obj == m_draggedPoints[0]) {
                        is_defining_point = true;
                        break;
                    }
                }
            } else if (auto* circle = dynamic_cast<Circle*>(constrain_target)) {
                for (GeoObject* obj : circle->sources()) {
                    if (obj == m_draggedPoints[0]) {
                        is_defining_point = true;
                        break;
                    }
                }
            }

            if (!is_defining_point) {
                setConstrainCandidate(constrain_target);
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

    Point* merge_target = m_mergeCandidate;
    GeoObject* constrain_target = m_constrainCandidate;
    setMergeCandidate(nullptr);
    setConstrainCandidate(nullptr);

    if ((merge_target != nullptr) && m_draggedPoints.size() == 1) {
        // Merge nur bei Einzelpunkt-Drag
        auto macro = std::make_unique<MacroCommand>(tr("Punkte zusammenführen"));
        for (auto& move : m_activeMoves) {
            macro->add(std::move(move));
        }
        macro->add(std::make_unique<MergePointsCommand>(m_ctx.adapter, PointPairForMergePointCommand{.survivor=merge_target, .absorbed=m_draggedPoints[0]}));
        m_ctx.commandStack->execute(std::move(macro));
    } else if (constrain_target != nullptr && m_draggedPoints.size() == 1) {
        // Punkt einschränken
        auto macro = std::make_unique<MacroCommand>(tr("Punkt einschränken"));
        for (auto& move : m_activeMoves) {
            macro->add(std::move(move));
        }
        if (auto* line = dynamic_cast<LinearObject*>(constrain_target)) {
            macro->add(std::make_unique<ConstrainPointToLineCommand>(m_ctx.adapter, m_draggedPoints[0], line));
        } else if (auto* circle = dynamic_cast<Circle*>(constrain_target)) {
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

    QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    Point* hit = pointAt(scene_pos);

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
            std::vector<GeoObject*> to_delete;
            std::vector<GeoObject*> to_delete_upward;
            std::function<void(GeoObject*)> collect = [&](GeoObject* obj) -> void {
                if (std::ranges::contains(to_delete, obj)) { 
                    return;
                }
                if (dynamic_cast<QGraphicsItem*>(obj)) {
                    return;
                }
                for (GeoObject* dep : obj->dependents()) {
                    collect(dep);
                }
                to_delete.push_back(obj);
            };

            std::function<void(GeoObject*)> collect_upward = [&](GeoObject* obj) -> void {
                if (std::ranges::contains(to_delete_upward, obj)) {
                    return;
                }
                if (dynamic_cast<QGraphicsItem*>(obj)) {
                    return;
                }

                std::vector<GeoObject*> geo_object_deps;
                for (GeoObject* dep : obj->dependents()) {
                    if (dynamic_cast<QGraphicsItem*>(dep)) {
                        continue;
                    }
                    geo_object_deps.push_back(dep);
                }

                if (!geo_object_deps.empty()) {
                    for (GeoObject* dep : geo_object_deps) {
                        collect_upward(dep);
                        to_delete_upward.push_back(obj);
                    }
                } else {
                    if (!dynamic_cast<QGraphicsItem*>(obj)) {
                        to_delete_upward.push_back(obj);
                    }
                }
            };

            for (GeoObject* obj : selection) {
                collect(obj);
                collect_upward(obj);
            }

            auto macro = std::make_unique<MacroCommand>(tr("Objekte löschen"));
            for (GeoObject* obj : to_delete_upward) {
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
    QAction* vis_action = menu.addAction(currentlyvisible ? tr("Ausblenden") : tr("Einblenden"));

    menu.addSeparator();

    // Farbe ändern
    QAction* color_action = menu.addAction(tr("Farbe ändern..."));

    QAction* chosen = menu.exec(event->globalPosition().toPoint());

    if (chosen == vis_action) {
        m_ctx.commandStack->execute(std::make_unique<SetVisibilityCommand>(m_ctx.adapter, obj, !currentlyvisible));
    } else if (chosen == color_action) {
        // Aktuelle Farbe ermitteln
        QColor current = Qt::black;
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            current = point_item->pen().color();
        } else if (auto* line_item = dynamic_cast<GeoLinearObjectItem*>(item)) {
            current = line_item->pen().color();
        } else if (auto* circle_item = dynamic_cast<GeoCircleItem*>(item)) {
            current = circle_item->pen().color();
        }

        QColor new_color = QColorDialog::getColor(current, m_ctx.drawingBoard, tr("Farbe wählen"));
        if (new_color.isValid()) {
            m_ctx.commandStack->execute(std::make_unique<SetColorCommand>(m_ctx.adapter, obj, new_color));
        }

    }
}

auto SelectTool::pointAt(const QPointF &scenePos) const -> Point * {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8, 8), QSize(16, 16)));
    for (QGraphicsItem *item : items) {
        if (auto* point_item = dynamic_cast<GeoPointItem *>(item)) {
            return point_item->point();
        }
    }
    return nullptr;
}

auto SelectTool::nearbyPoint(const QPointF &scenePos, Point *exclude) const -> Point* {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(10, 10), QSize(20, 20)));
    for (QGraphicsItem *item : items) {
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            Point* point = point_item->point();
            if (point != exclude && point->isValid() && point_item->isVisible()) {
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
        if (auto* geo_item = dynamic_cast<GeoGraphicsItem*>(item)) {
            if (typeid(*geo_item) == type && geo_item->contains(scenePos)) {
                return geo_item;
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
        auto* point_item = dynamic_cast<GeoPointItem*>(item);
        if ((point_item == nullptr) || !isDraggable(point_item->point())) {
            continue;
        }

        m_draggedPoints.push_back(point);

        Point* radius_point = m_ctx.adapter->radiusPointFor(point);
        if ((radius_point != nullptr) && !sel.contains(radius_point)) {
            // Kreismittelpunkt mitbewegen
            m_activeMoves.push_back(std::make_unique<MoveCenterCommand>(point, radius_point, point->x(), point->y()));
        } else if (radius_point == nullptr) {
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
    QRect  view_rect  = QRect(m_rubberStart, viewPos).normalized();
    QRectF scene_rect = m_ctx.drawingBoard->mapToScene(view_rect).boundingRect();

    // Alle Items im Rechteck selektieren
    const auto items = m_ctx.drawingBoard->scene()->items(
        scene_rect, Qt::ContainsItemBoundingRect);

    for (QGraphicsItem* item : items) {
        if (auto* geo_item = dynamic_cast<GeoGraphicsItem*>(item)) {
            if ((geo_item->geoObject() != nullptr) && geo_item->geoObject()->isValid()) {
                m_ctx.adapter->select(geo_item->geoObject());
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
