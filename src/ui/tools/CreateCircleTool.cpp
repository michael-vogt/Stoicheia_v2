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

    bool snapActive = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

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
        CreatePointCommand* centerPointCmd = nullptr;
        Point* centerPoint = m_centerPoint;
        if (m_centerIsNew) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, m_centerScenePos.x(), m_centerScenePos.y());
            centerPointCmd = cmd.get();
            centerPoint = nullptr;
            macro->add(std::move(cmd));
        }

        // zweiter Punkt
        CreatePointCommand* radiusPointCmd = nullptr;
        Point* radiusPoint = pointAt(scenePos);

        if (radiusPoint == nullptr) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y());
            radiusPointCmd = cmd.get();
            radiusPoint = nullptr;
            macro->add(std::move(cmd));
        }

        macro->add(std::make_unique<CreateCircleCommand>(m_ctx.adapter,
             CreatePointCommandPairForCircle{.centerPointCmd=centerPointCmd, .radiusPointCmd=radiusPointCmd},
             PointPairForCircle{.center=centerPoint, .radiusPoint=radiusPoint}
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

    bool snapActive = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    m_preview->setRect(computePreviewCircle(snapped));

    m_ctx.drawingBoard->viewport()->update();
    event->accept();
}

auto CreateCircleTool::computePreviewCircle(const QPointF &scenePos) const -> QRectF {
    if (m_centerPoint == nullptr) {
        return {};
    }

    const QPointF centerPoint(m_centerPoint->x(), m_centerPoint->y());
    const QPointF radiusPoint = scenePos;

    const double delta_x = radiusPoint.x() - centerPoint.x();
    const double delta_y = radiusPoint.y() - centerPoint.y();
    const double radius = sqrt((delta_x * delta_x) + (delta_y * delta_y));

    return QRectF(centerPoint.x() - radius, centerPoint.y() - radius, 2 * radius, 2 * radius);
}

auto CreateCircleTool::pointAt(const QPointF &scenePos) const -> Point* {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8,8), QSizeF(16,16)));
    for (QGraphicsItem* item : items) {
        if (auto* pointItem = dynamic_cast<GeoPointItem*>(item)) {
            return pointItem->point();
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
