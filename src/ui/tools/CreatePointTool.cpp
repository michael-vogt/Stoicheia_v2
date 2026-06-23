#include "CreatePointTool.h"

#include "ui/DrawingBoard.h"
#include "ui/commands/CommandStack.h"
#include "ui/commands/CreatePointCommand.h"
#include "ui/commands/ConstrainPointCommand.h"
#include "../dialogs/CoordinateInputDialog.h"
#include "../../Constants.h"
#include <memory>
#include <qevent.h>

#include "ui/commands/MacroCommand.h"


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

    bool snap_active = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scene_pos, snap_active);

    // Liegt der Klick auf einer Geraden odr einem Kreis?
    LinearObject* line = m_ctx.hitTest->linearObjectAt(scene_pos);
    Circle* circle = m_ctx.hitTest->circleAt(scene_pos);

    if (line != nullptr || circle != nullptr) {
        // Freien Punkt erzeugen, dann sofort einschränken - als MacroCommand damit ein einzelnes Undo beide Schritte rückgängig macht
        auto create_cmd = std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y());
        CreatePointCommand* create_raw = create_cmd.get();

        auto macro = std::make_unique<MacroCommand>((line != nullptr) ? tr("Punkt auf Gerade") : tr("Punkt auf Kreis"));
        macro->add(std::move(create_cmd));

        // execute() jetzt schon, damit createRaw->point() gesetzt ist
        create_raw->execute();

        if (line != nullptr) {
            macro->add(std::make_unique<ConstrainPointToLineCommand>(m_ctx.adapter, create_raw->point(), line));
        } else {
            macro->add(std::make_unique<ConstrainPointToCircleCommand>(m_ctx.adapter, create_raw->point(), circle));
        }

        m_ctx.commandStack->execute(std::move(macro));
    } else {
        m_ctx.commandStack->execute(std::make_unique<CreatePointCommand>(m_ctx.adapter, snapped.x(), snapped.y()));
    }

    event->accept();
}

void CreatePointTool::mouseMoveEvent(QMouseEvent *event) {
    bool snap_active = (event->modifiers() & Qt::AltModifier) != 0;
    QPointF scene_pos = m_ctx.drawingBoard->mapToScene(event->pos());
    QPointF snapped = m_ctx.snapHelper->snap(scene_pos, snap_active);

    m_lastMousePos = snapped;
    updatePreview(snapped);
    event->accept();
}

void CreatePointTool::keyPressEvent(QKeyEvent *event) {
    // Wenn bereits ein Dialog offen ist, tue nichts. Der Dialog kümmert sich um die Tastatureingabe selbst
    if (m_dialogOpen) {
        event->ignore();
        return;
    }

    // Zahl, Minus oder Komma gedrückt -> Dialog öffnen
    const QString text = event->text();
    if (!text.isEmpty() && (text[0].isDigit() || text[0] == '-' || text[0] == ',')) {
        openCoordinateDialog(m_lastMousePos);
        event->accept();
        return;
    }
    event->ignore();
}

void CreatePointTool::openCoordinateDialog(const QPointF& scenePos) {
    m_dialogOpen = true;
    CoordinateInputDialog dlg(m_ctx.drawingBoard);
    dlg.setCoordinates(scenePos.x(), scenePos.y());
    if (dlg.exec() == QDialog::Accepted) {
        QPointF pos = dlg.coordinates();
        m_ctx.commandStack->execute(std::make_unique<CreatePointCommand>(m_ctx.adapter, pos.x(), pos.y()));
    }
    m_dialogOpen = false;
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