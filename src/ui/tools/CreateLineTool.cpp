#include "CreateLineTool.h"

#include "Enums.h"
#include "LinearObjectType.h"
#include "Structs.h"
#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreateLineCommand.h"
#include "ui/commands/CreatePointCommand.h"
#include "ui/commands/MacroCommand.h"
#include "../../Constants.h"


using namespace Constants;

CreateLineTool::CreateLineTool(const ToolContext &ctx, LinearObjectType type) : Tool(ctx), m_type(type) {}

void CreateLineTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->viewport()->setMouseTracking(true);
    m_ctx.drawingBoard->showStatusLeft(tr("Ersten Punkt klicken - L: Gerade, R: Halbgerade, S: Strecke"));
}

void CreateLineTool::deactivate() {
    m_ctx.drawingBoard->showStatusLeft("");
    m_ctx.drawingBoard->viewport()->setMouseTracking(false);
    removePreview();
    if ((m_firstPoint != nullptr) && m_firstIsNew) {
        m_ctx.adapter->remove(m_firstPoint);
    }

    m_firstPoint = nullptr;
    m_firstIsNew = false;
}

void CreateLineTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    bool snap_active = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scene_pos, snap_active);

    if (m_firstPoint == nullptr) {
        // Erster Klick: bestehenden Punkt nehmen oder neuen erzeugen
        m_firstPoint = pointAt(snapped);
        if (m_firstPoint == nullptr) {
            m_firstPoint = m_ctx.adapter->geoScene()->create<Point>(snapped.x(), snapped.y());
            m_ctx.adapter->addPoint(m_firstPoint);
            m_firstIsNew = true;
            m_firstScenePos = snapped;
        }

        m_ctx.drawingBoard->showStatusLeft(tr("Zweiten Punkt klicken - L: Gerade, R: Halbgerade, S: Strecke"));

        // Vorschaulinie starten
        m_preview = new QGraphicsLineItem(computePreviewLine(snapped));
        m_preview->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        m_ctx.drawingBoard->scene()->addItem(m_preview);
    } else {
        // Zweiter Klick: Endpunkt + Linie erzeugen
        if (m_firstIsNew) {
            m_ctx.adapter->remove(m_firstPoint);
        }

        QString description;
        if (m_type == LinearObjectType::Line) {
            description = tr("Gerade erstellen");
        } else if (m_type == LinearObjectType::Ray) {
            description = tr("Halbgerade erstellen");
        } else {
            description = tr("Strecke erstellen");
        }
        auto macro = std::make_unique<MacroCommand>(description);

        // Ersten Punkt ins Macro, falls wir ihn erstellt haben
        CreatePointCommand* p1_cmd = nullptr;
        Point* point1 = m_firstPoint;

        if (m_firstIsNew) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, m_firstScenePos.x(), m_firstScenePos.y());
            p1_cmd = cmd.get();
            point1 = nullptr;
            macro->add(std::move(cmd));
        }

        // Zweiter Punkt
        CreatePointCommand* p2_cmd = nullptr;
        Point* point2 = pointAt(scene_pos);

        if (point2 == nullptr) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y());
            p2_cmd = cmd.get();
            point2 = nullptr;
            macro->add(std::move(cmd));
        }

        macro->add(std::make_unique<CreateLineCommand>(m_ctx.adapter, CreatePointCommandPairForLinearObject{.firstPoint=p1_cmd, .secondPoint=p2_cmd}, PointPairForLinearObject{.point1=point1, .point2=point2}, m_type));
        m_ctx.commandStack->execute(std::move(macro));

        removePreview();
        m_firstPoint = nullptr;
        m_firstIsNew = false;
    }
    event->accept();
}

void CreateLineTool::mouseMoveEvent(QMouseEvent *event) {
    if ((m_preview == nullptr) || (m_firstPoint == nullptr)) {
        event->ignore();
        return;
    }

    bool snap_active = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scene_pos, snap_active);

    m_lastScenePos = snapped;
    m_preview->setLine(computePreviewLine(snapped));
    m_ctx.drawingBoard->viewport()->update();
    event->accept();
}

void CreateLineTool::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_L:
            setType(LinearObjectType::Line);
            emit m_ctx.drawingBoard->toolChanged(ToolType::CreateLine);
            event->accept();
            return;
        case Qt::Key_R:
            setType(LinearObjectType::Ray);
            emit m_ctx.drawingBoard->toolChanged(ToolType::CreateRay);
            event->accept();
            return;
        case Qt::Key_S:
            setType(LinearObjectType::Segment);
            emit m_ctx.drawingBoard->toolChanged(ToolType::CreateSegment);
            event->accept();
            return;
        default:
            event->ignore();
    }
}

auto CreateLineTool::pointAt(const QPointF &scenePos) const -> Point* {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8,8), QSizeF(16,16)));
    for (QGraphicsItem *item : items) {
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            return point_item->point();
        }
    }
    return nullptr;
}

void CreateLineTool::setType(const LinearObjectType type) {
    m_type = type;
    ToolType tool_type = ToolType::CreateSegment;
    if (type == LinearObjectType::Line) {
        tool_type = ToolType::CreateLine;
    } else if (type == LinearObjectType::Ray) {
        tool_type = ToolType::CreateRay;
    }
    
    m_ctx.drawingBoard->updateToolType(tool_type);
    if (m_preview != nullptr) {
        m_preview->setLine(computePreviewLine(m_lastScenePos));
    }
}

auto CreateLineTool::computePreviewLine(const QPointF &endPos) const -> QLineF {
    if (m_firstPoint == nullptr) { 
        return {};
    }

    QPointF point1(m_firstPoint->x(), m_firstPoint->y());
    QPointF point2 = endPos;

    double delta_x = point2.x() - point1.x();
    double delta_y = point2.y() - point1.y();
    double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));
    if (len < NumericConstants::DOUBLE_EPS) { 
        return {point1, point2};
    }

    QPointF dir(delta_x / len, delta_y / len);

    switch (m_type) {
        case LinearObjectType::Segment:
            return {point1, point2};
        case LinearObjectType::Ray:
            return {point1, point1 + dir * GeometryConstants::LINE_EXTENT};
        case LinearObjectType::Line:
            return {point1 - dir * GeometryConstants::LINE_EXTENT, point1 + dir * GeometryConstants::LINE_EXTENT};
    }
    return {point1, point2};
}

void CreateLineTool::removePreview() {
    if (m_preview != nullptr) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
        delete m_preview;
        m_preview = nullptr;
    }
}
