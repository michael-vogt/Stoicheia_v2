#include "CreatePointTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreatePointCommand.h"
#include "../dialogs/CoordinateInputDialog.h"
#include "../../Constants.h"
#include <memory>
#include <qevent.h>


using namespace Constants;

CreatePointTool::CreatePointTool(const ToolContext &ctx) : Tool(ctx) {}

void CreatePointTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->showStatusLeft(tr("Punkt durch Klicken hinzufügen oder Koordinaten eingeben"));
}

void CreatePointTool::deactivate() {
    if (m_preview != nullptr) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
    }
    m_ctx.drawingBoard->showStatusLeft("");
}

void CreatePointTool::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    bool snapActive = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    m_ctx.commandStack->execute(std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y()));
    event->accept();
}

void CreatePointTool::mouseMoveEvent(QMouseEvent *event) {
    bool snapActive = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scenePos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scenePos, snapActive);

    m_lastMousePos = snapped;
    updatePreview(snapped);
    event->accept();
}

void CreatePointTool::keyPressEvent(QKeyEvent *event) {
    // Zahl oder Minus gedrückt -> Dialog öffnen
    const QString text = event->text();
    if (!text.isEmpty() && (text[0].isDigit() || text[0] == '-' || text[0] == ',')) {
        openCoordinateDialog(m_lastMousePos);
        event->accept();
        return;
    }
    event->ignore();
}

void CreatePointTool::openCoordinateDialog(const QPointF& scenePos) {
    CoordinateInputDialog dlg(m_ctx.drawingBoard);
    dlg.setCoordinates(scenePos.x(), scenePos.y());
    if (dlg.exec() == QDialog::Accepted) {
        QPointF pos = dlg.coordinates();
        m_ctx.commandStack->execute(std::make_unique<CreatePointCommand>(m_ctx.adapter, pos.x(), pos.y()));
    }
}

void CreatePointTool::updatePreview(const QPointF& scenePos) {
    if (m_preview == nullptr) {
        m_preview = new QGraphicsEllipseItem();
        m_preview->setPen(QPen(Qt::gray, DrawingConstants::PENWIDTH_NORMAL));
        m_preview->setBrush(QBrush(Qt::white));
        // Preview nicht durch HitTest treffbar
        m_preview->setFlag(QGraphicsItem::ItemIsSelectable, false);
        m_ctx.drawingBoard->scene()->addItem(m_preview);
    }

    m_preview->setRect(scenePos.x() - DrawingConstants::POINT_RADIUS,
                       scenePos.y() - DrawingConstants::POINT_RADIUS,
                       DrawingConstants::POINT_RADIUS * 2, 2 * DrawingConstants::POINT_RADIUS);
}

void CreatePointTool::removePreview() {
    if (m_preview != nullptr) {
        m_ctx.drawingBoard->scene()->removeItem(m_preview);
        delete m_preview;
        m_preview = nullptr;
    }
}