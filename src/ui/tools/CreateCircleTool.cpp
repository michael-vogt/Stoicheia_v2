#include "CreateCircleTool.h"

#include "Structs.h"
#include "ui/DrawingBoard.h"
#include "ui/commands/CreateCircleCommand.h"
#include "ui/commands/CreatePointCommand.h"
#include "ui/commands/MacroCommand.h"

CreateCircleTool::CreateCircleTool(const ToolContext& ctx)
: Tool(ctx)
{}

void CreateCircleTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->viewport()->setMouseTracking(true);
    m_ctx.drawingBoard->showStatusLeft(tr("Mittelpunkt klicken"));
}

void CreateCircleTool::deactivate() {
    m_ctx.drawingBoard->showStatusLeft("");
    m_ctx.drawingBoard->viewport()->setMouseTracking(false);
    removePreview();
    if ((m_centerPoint != nullptr) && m_centerIsNew) {
        m_ctx.adapter->remove(m_centerPoint);
    }

    m_centerPoint = nullptr;
    m_centerIsNew = false;
}

void CreateCircleTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    bool snap_active = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scene_pos, snap_active);

    if (m_centerPoint == nullptr) {
        // Erster Klick: Bestehenden Punkt nehmen oder neuen erzeugen
        m_centerPoint = pointAt(snapped);
        if (m_centerPoint == nullptr) {
            m_centerPoint = m_ctx.adapter->geoScene()->create<Point>(snapped.x(), snapped.y());
            m_ctx.adapter->addPoint(m_centerPoint);
            m_centerIsNew = true;
            m_centerScenePos = snapped;
        }

        m_ctx.drawingBoard->showStatusLeft(tr("Punkt auf dem Kreis klicken"));
        m_preview = new QGraphicsEllipseItem(m_centerPoint->x(), m_centerPoint->y(), 0, 0);
        m_preview->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        m_ctx.drawingBoard->scene()->addItem(m_preview);
    } else {
        // Zweiter Klick: Kreis erzeugen
        if (m_centerIsNew) {
            m_ctx.adapter->remove(m_centerPoint);
        }

        auto macro = std::make_unique<MacroCommand>(tr("Kreis erstellen"));

        // erster Punkt ins Macro, falls wir ihn erstellt haben
        CreatePointCommand* center_point_cmd = nullptr;
        Point* center_point = m_centerPoint;
        if (m_centerIsNew) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, m_centerScenePos.x(), m_centerScenePos.y());
            center_point_cmd = cmd.get();
            center_point = nullptr;
            macro->add(std::move(cmd));
        }

        // zweiter Punkt
        CreatePointCommand* radius_point_cmd = nullptr;
        Point* radius_point = pointAt(scene_pos);

        if (radius_point == nullptr) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y());
            radius_point_cmd = cmd.get();
            radius_point = nullptr;
            macro->add(std::move(cmd));
        }

        macro->add(std::make_unique<CreateCircleCommand>(m_ctx.adapter,
             CreatePointCommandPairForCircle{.centerPointCmd=center_point_cmd, .radiusPointCmd=radius_point_cmd},
             PointPairForCircle{.center=center_point, .radiusPoint=radius_point}
            ));
        m_ctx.commandStack->execute(std::move(macro));

        removePreview();
        m_centerPoint = nullptr;
        m_centerIsNew = false;
    }

    event->accept();
}

void CreateCircleTool::mouseMoveEvent(QMouseEvent *event) {
    if ((m_preview == nullptr) || (m_centerPoint == nullptr)) {
        event->ignore();
        return;
    }

    bool snap_active = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scene_pos, snap_active);

    m_preview->setRect(computePreviewCircle(snapped));

    m_ctx.drawingBoard->viewport()->update();
    event->accept();
}

auto CreateCircleTool::computePreviewCircle(const QPointF &scenePos) const -> QRectF {
    if (m_centerPoint == nullptr) {
        return {};
    }

    const QPointF center_point(m_centerPoint->x(), m_centerPoint->y());
    const QPointF radius_point = scenePos;

    const double delta_x = radius_point.x() - center_point.x();
    const double delta_y = radius_point.y() - center_point.y();
    const double radius = sqrt((delta_x * delta_x) + (delta_y * delta_y));

    return {center_point.x() - radius, center_point.y() - radius, 2 * radius, 2 * radius};
}

auto CreateCircleTool::pointAt(const QPointF &scenePos) const -> Point* {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8,8), QSizeF(16,16)));
    for (QGraphicsItem* item : items) {
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            return point_item->point();
        }
    }
    return nullptr;
}

void CreateCircleTool::removePreview() {
    if (m_preview != nullptr) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
        delete m_preview;
        m_preview = nullptr;
    }
}
