#include "CreateLineTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreateLineCommand.h"
#include "ui/commands/CreatePointCommand.h"
#include "ui/commands/MacroCommand.h"

CreateLineTool::CreateLineTool(const ToolContext &ctx, LinearObjectType type) : Tool(ctx), m_type(type) {}

void CreateLineTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->viewport()->setMouseTracking(true);
    m_ctx.drawingBoard->showStatus(QObject::tr("Ersten Punkt klicken - L: Gerade, R: Halbgerade, S: Strecke"));
}

void CreateLineTool::deactivate() {
    m_ctx.drawingBoard->showStatus("");
    m_ctx.drawingBoard->viewport()->setMouseTracking(false);
    removePreview();
    if (m_firstPoint && m_firstIsNew)
        m_ctx.adapter->remove(m_firstPoint);

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
            m_firstPoint = m_ctx.adapter->geoScene()->create<Point>(snapped.x(), snapped.y());
            m_ctx.adapter->addPoint(m_firstPoint);
            m_firstIsNew = true;
            m_firstScenePos = snapped;
        }

        m_ctx.drawingBoard->showStatus(QObject::tr("Zweiten Punkt klicken - L: Gerade, R: Halbgerade, S: Strecke"));

        // Vorschaulinie starten
        m_preview = new QGraphicsLineItem(computePreviewLine(snapped));
        m_preview->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        m_ctx.drawingBoard->scene()->addItem(m_preview);
    } else {
        // Zweiter Klick: Endpunkt + Linie erzeugen
        if (m_firstIsNew)
            m_ctx.adapter->remove(m_firstPoint);

        auto macro = std::make_unique<MacroCommand>(QObject::tr(
            m_type == LinearObjectType::Line ? "Gerade erstellen" :
            m_type == LinearObjectType::Ray ?  "Halbgerade erstellen" :
                                               "Strecke erstellen"));

        // Ersten Punkt ins Macro, falls wir ihn erstellt haben
        CreatePointCommand* p1Cmd = nullptr;
        Point* p1 = m_firstPoint;

        if (m_firstIsNew) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, m_firstScenePos.x(), m_firstScenePos.y());
            p1Cmd = cmd.get();
            p1 = nullptr;
            macro->add(std::move(cmd));
        }

        // Zweiter Punkt
        CreatePointCommand* p2Cmd = nullptr;
        Point* p2 = pointAt(scenePos);

        if (!p2) {
            auto cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y());
            p2Cmd = cmd.get();
            p2 = nullptr;
            macro->add(std::move(cmd));
        }

        macro->add(std::make_unique<CreateLineCommand>(m_ctx.adapter, p1Cmd, p2Cmd, p1, p2, m_type));
        m_ctx.commandStack->execute(std::move(macro));

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
