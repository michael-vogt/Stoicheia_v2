#include "CreateLineTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreateLineCommand.h"
#include "ui/commands/CreatePointCommand.h"

CreateLineTool::CreateLineTool(const ToolContext &ctx, LinearObjectType type) : Tool(ctx), m_type(type) {}

void CreateLineTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->viewport()->setMouseTracking(true);
}

void CreateLineTool::deactivate() {
    m_ctx.drawingBoard->viewport()->setMouseTracking(false);
    removePreview();
    // Wurde ein erster Punkt erzeugt aber kein zweiter -> Rückgängig machen
    if (m_firstPoint && m_firstIsNew) {
        m_ctx.adapter->remove(m_firstPoint);
    }

    m_firstPoint = nullptr;
    m_firstIsNew = false;
}

QCursor CreateLineTool::cursor() const {
    return Qt::CrossCursor;
}

Point* CreateLineTool::pointAt(const QPointF &scenePos) const {
    const auto items = m_ctx.drawingBoard->scene()->items(
        QRectF(scenePos - QPointF(8,8), QSizeF(16,16)));
    for (QGraphicsItem *item : items) {
        if (auto* pi = dynamic_cast<GeoPointItem*>(item))
            return pi->point();
    }
    return nullptr;
}

void CreateLineTool::setType(const LinearObjectType type) {
    m_type = type;
    if (m_preview)
        m_preview->setLine(computePreviewLine(m_lastScenePos));
}

void CreateLineTool::removePreview() {
    if (m_preview) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
        delete m_preview;
        m_preview = nullptr;
    }
}

void CreateLineTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    bool snapActive = event->modifiers() & Qt::AltModifier;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    if (!m_firstPoint) {
        // Erster Klick: bestehenden Punkt nehmen oder neuen erzeugen
        m_firstPoint = pointAt(snapped);
        if (!m_firstPoint) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y());
            auto* rawCmd = cmd.get();
            m_ctx.commandStack->execute(std::move(cmd));
            m_firstPoint = rawCmd->point();
            m_firstIsNew = true;
        }

        // Vorschaulinie starten
        m_preview = new QGraphicsLineItem(
            QLineF(m_firstPoint->x(), m_firstPoint->y(), snapped.x(), snapped.y()));
        m_preview->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        m_ctx.drawingBoard->scene()->addItem(m_preview);
    } else {
        // Zweiter Klick: Endpunkt + Linie erzeugen
        Point* second = pointAt(snapped);
        if (!second) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y());
            auto* rawCmd = cmd.get();
            m_ctx.commandStack->execute(std::move(cmd));
            second = rawCmd->point();
        }

        // Linie als Command erzeugen
        m_ctx.commandStack->execute(std::make_unique<CreateLineCommand>(m_ctx.adapter, m_firstPoint, second, m_type));
        removePreview();
        m_firstPoint = nullptr;
        m_firstIsNew = false;
    }
    event->accept();
}

void CreateLineTool::mouseMoveEvent(QMouseEvent *event) {
    if (!m_preview || !m_firstPoint) {
        event->ignore();
        return;
    }

    bool snapActive = event->modifiers() & Qt::AltModifier;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    //m_preview->setLine(QLineF(m_firstPoint->x(), m_firstPoint->y(), snapped.x(), snapped.y()));
    m_lastScenePos = snapped;
    m_preview->setLine(computePreviewLine(snapped));
    m_ctx.drawingBoard->viewport()->update();
    event->accept();
}

void CreateLineTool::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_L:
            setType(LinearObjectType::Line);
            event->accept();
            return;
        case Qt::Key_R:
            setType(LinearObjectType::Ray);
            event->accept();
            return;
        case Qt::Key_S:
            setType(LinearObjectType::Segment);
            event->accept();
            return;
        default:
            event->ignore();
    }
}

QLineF CreateLineTool::computePreviewLine(const QPointF &endPos) const {
    if (!m_firstPoint) return {};

    QPointF p1(m_firstPoint->x(), m_firstPoint->y());
    QPointF p2 = endPos;

    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    double len = std::sqrt(dx*dx + dy*dy);
    if (len < 1e-10) return QLineF(p1, p2);

    QPointF u(dx / len, dy / len);
    const double extent = 10000.0;

    switch (m_type) {
        case LinearObjectType::Segment:
            return QLineF(p1, p2);
        case LinearObjectType::Ray:
            return QLineF(p1, p1 + u * extent);
        case LinearObjectType::Line:
            return QLineF(p1 - u * extent, p1 + u * extent);
    }
    return QLineF(p1, p2);
}
