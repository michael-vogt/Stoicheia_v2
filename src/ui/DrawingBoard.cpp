#include "DrawingBoard.h"

#include <iostream>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStatusBar>
#include <QTimer>
#include <QShortcut>

#include "MainWindow.h"
#include "geometry/Point.h"
#include "tools/CreatePointTool.h"
#include "tools/CreateLineTool.h"
#include "tools/CreateCircleTool.h"
#include "tools/CreateIntersectionTool.h"
#include "tools/CreateMidpointTool.h"
#include "tools/CreateParallelTool.h"
#include "tools/CreatePerpendicularTool.h"
#include "tools/CreatePerpendicularFootTool.h"

DrawingBoard::DrawingBoard(QWidget *parent) : QGraphicsView(parent), m_adapter(&m_geoScene, &m_qtScene) {
    setScene(&m_qtScene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setBackgroundBrush(QColor(245, 245, 245));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSceneRect(-10000, -10000, 20000, 20000);

    QTransform transform;
    transform.scale(1.0, -1.0);
    setTransform(transform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setDragMode(QGraphicsView::NoDrag);
    setFocusPolicy(Qt::StrongFocus);

    auto escShortcut = std::make_unique<QShortcut>(Qt::Key_Escape, this);
    escShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(escShortcut.get(), &QShortcut::activated, [this]() {
        if (m_shortcutMode != ShortcutMode::None) {
            setShortcutMode(ShortcutMode::None);
            return;
        }
        if (m_activeTool) {
            QKeyEvent escEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
            m_activeTool->keyPressEvent(&escEvent);
            if (escEvent.isAccepted()) return;
        }
        emit escapePressed();
    });
    escShortcut.release();

    connect(&m_adapter, &SceneAdapter::selectionChanged, [this]() {
        viewport()->update();
    });
}

void DrawingBoard::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawBackground(painter, rect);
    m_grid.drawBackground(painter, rect);
}

void DrawingBoard::drawForeground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawForeground(painter, rect);
    if (!m_grid.isVisible()) return;

    painter->save();
    painter->setTransform(QTransform());

    auto toViewport = [this](QPointF p) -> QPointF {
        return viewport()->mapFrom(this, mapFromScene(p));
    };

    m_grid.drawLabels(painter, toViewport, viewport()->width(), viewport()->height());
    painter->restore();
}

void DrawingBoard::setGridVisible(bool visible) {
    m_gridVisible = visible;
    viewport()->update();
}

void DrawingBoard::setGridSpacing(double spacing) {
    m_gridSpacing = spacing;
    viewport()->update();
}

/*
   ─────────────────────────────────────────────────────────────────────────────
   ── Zoom ─────────────────────────────────────────────────────────────────────
   ─────────────────────────────────────────────────────────────────────────────
*/

void DrawingBoard::wheelEvent(QWheelEvent *event) {
    // Pan hat Vorrang wenn Space gedrückt
    if (m_spacePressed) {
        event->ignore();
        return;
    }
    double factor = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
    scale(factor, factor);
    event->accept();
}

/*
   ─────────────────────────────────────────────────────────────────────────────
   ── Maus- und Tastaturereignisse: Pan zuerst, dann Tool ──────────────────────
   ─────────────────────────────────────────────────────────────────────────────
*/

void DrawingBoard::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && m_spacePressed)) {
        m_panning = true;
        m_panStart = event->pos();
        viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
        emit statusMessageChanged(tr("Panning"));
        return;
    }

    // An aktives Tool delegieren
    if (m_activeTool) {
        m_activeTool->mousePressEvent(event);
        if (event->isAccepted())
            return;
    }

    QGraphicsView::mousePressEvent(event);
}

void DrawingBoard::mouseMoveEvent(QMouseEvent *event) {
    if (m_panning) {
        QPoint delta = event->pos() - m_panStart;
        m_panStart = event->pos();

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        event->accept();
        viewport()->update();
        return;
    }

    if (m_activeTool) {
        m_activeTool->mouseMoveEvent(event);
        if (event->isAccepted())
            return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void DrawingBoard::mouseReleaseEvent(QMouseEvent *event) {
    if (m_panning) {
        m_panning = false;
        viewport()->setCursor(m_spacePressed ? Qt::OpenHandCursor : Qt::ArrowCursor);
        event->accept();
        emit statusMessageChanged("");
        return;
    }

    if (m_activeTool) {
        m_activeTool->mouseReleaseEvent(event);
        if (event->isAccepted())
            return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void DrawingBoard::keyPressEvent(QKeyEvent *event) {
    // Aktiver Shortcut-Modus hat absoluten Vorrang
    if (m_shortcutMode != ShortcutMode::None) {
        handleShortcutKey(event);
        if (event->isAccepted()) return;
    }

    // Tool zuerst
    if (m_activeTool) {
        m_activeTool->keyPressEvent(event);
        if (event->isAccepted())
            return;
    }

    if (event->key() == Qt::Key_Alt) {
        showStatus(tr("Snapping"));
    }

    // Undo/Redo
    if (event->matches(QKeySequence::Undo)) {
        m_commandStack.undo();
        event->accept();
        return;
    }

    if (event->matches(QKeySequence::Redo)) {
        m_commandStack.redo();
        event->accept();
        return;
    }
    
    // Pan-Modus
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        showStatus(tr("Panning"));
        m_spacePressed = true;
        viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_R && !event->isAutoRepeat()) {
        resetView();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_NumberSign && !event->isAutoRepeat()) {
        m_grid.setVisible(!m_grid.isVisible());
        event->accept();
        return;
    }

    handleShortcutKey(event);

    QGraphicsView::keyPressEvent(event);
}

void DrawingBoard::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Alt) {
        emit statusMessageChanged("");
    }

    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        emit statusMessageChanged("");
        m_spacePressed = false;
        if (!m_panning)
            viewport()->setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }

    QGraphicsView::keyReleaseEvent(event);
}



/*
   ─────────────────────────────────────────────────────────────────────────────
   ── Zentrierung ───────────────────────────────────────────────────────────────
   ─────────────────────────────────────────────────────────────────────────────
*/

void DrawingBoard::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    if (m_resizeCount < 2) {
        ++m_resizeCount;
        QPointF origin = mapFromScene(QPointF(0, 0));
        QPointF center = viewport()->rect().center();
        QPointF delta  = origin - center;

        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() + static_cast<int>(delta.x()));
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + static_cast<int>(delta.y()));
    }
    viewport()->update();
}

void DrawingBoard::resetView() {
    setTransformationAnchor(QGraphicsView::NoAnchor);
    QTransform t = transform();
    double scaleX = t.m11(); // in case of rotation and/or shear: std::sqrt(t.m11() * t.m11() + t.m21() * t.m21());
    double scaleY = t.m22(); // in case of rotation and/or shear: std::sqrt(t.m22() * t.m22() + t.m12() * t.m12());
    centerOn(0, 0);
    scale(1.0 / scaleX, -1.0 / scaleY);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void DrawingBoard::setShortcutMode(ShortcutMode mode) {
    m_shortcutMode = mode;
    emit shortcutModeChanged(mode);
    switch (mode) {
        case ShortcutMode::None:
            showStatus("");
            break;
        case ShortcutMode::Geometry:
            showStatus(tr("Geometrie: [P] Punkt [L] Gerade [R] Halbgerade [S] Strecke [C] Kreis [Esc] Abbrechen"));
            break;
        case ShortcutMode::Construction:
            showStatus(tr("Konstruktion: [S] Schnittpunkt [M] Mittelpunkt [P] Parallele [E] Senkrechte [L] Lotfußpunkt [Esc] Abbrechen"));
            break;
    }
}

void DrawingBoard::handleShortcutKey(QKeyEvent* event) {
    if (m_shortcutMode == ShortcutMode::None) {
        switch(event->key()) {
            case Qt::Key_G:
                setShortcutMode(ShortcutMode::Geometry);
                event->accept();
                return;
            case Qt::Key_K:
                setShortcutMode(ShortcutMode::Construction);
                event->accept();
                return;
        }
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        setShortcutMode(ShortcutMode::None);
        event->accept();
        return;
    }

    if (m_shortcutMode == ShortcutMode::Geometry) {
        setShortcutMode(ShortcutMode::None);
        switch (event->key()) {
            case Qt::Key_P:
                setTool<CreatePointTool>(ToolType::CreatePoint);
                break;
            case Qt::Key_L:
                setTool<CreateLineTool>(ToolType::CreateLine, LinearObjectType::Line);
                break;
            case Qt::Key_R:
                setTool<CreateLineTool>(ToolType::CreateRay, LinearObjectType::Ray);
                break;
            case Qt::Key_S:
                setTool<CreateLineTool>(ToolType::CreateSegment, LinearObjectType::Segment);
                break;
            case Qt::Key_C:
                setTool<CreateCircleTool>(ToolType::CreateCircle);
                break;
            default:
                event->ignore();
                return;
        }
        event->accept();
        return;
    }

    if (m_shortcutMode == ShortcutMode::Construction) {
        setShortcutMode(ShortcutMode::None);
        switch (event->key()) {
            case Qt::Key_S:
                setTool<CreateIntersectionTool>(ToolType::CreateIntersection);
                break;
            case Qt::Key_M:
                setTool<CreateMidpointTool>(ToolType::CreateMidpoint);
                break;
            case Qt::Key_P:
                setTool<CreateParallelTool>(ToolType::CreateParallel);
                break;
            case Qt::Key_E:
                setTool<CreatePerpendicularTool>(ToolType::CreatePerpendicular);
                break;
            case Qt::Key_L:
                setTool<CreatePerpendicularFootTool>(ToolType::CreatePerpendicularFoot);
                break;
            default:
                event->ignore();
                return;
        }
        event->accept();
        return;
    }
}

void DrawingBoard::updateToolType(ToolType type) {
    m_activeToolType = type;
    emit toolChanged(type);
}
