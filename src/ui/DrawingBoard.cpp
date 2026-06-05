#include "DrawingBoard.h"

#include <iostream>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStatusBar>
#include <QTimer>
#include <QShortcut>

//#include "MainWindow.h"
#include "commands/CopyCommand.h"
#include "dialogs/AppSettings.h"
#include "geometry/Point.h"
#include "tools/CreatePointTool.h"
#include "tools/CreateLineTool.h"
#include "tools/CreateCircleTool.h"
#include "tools/CreateIntersectionTool.h"
#include "tools/CreateMidpointTool.h"
#include "tools/CreateParallelTool.h"
#include "tools/CreatePerpendicularTool.h"
#include "tools/CreatePerpendicularFootTool.h"
#include "tools/SelectTool.h"

DrawingBoard::DrawingBoard(QWidget *parent) : QGraphicsView(parent), m_adapter(&m_geoScene, &m_qtScene) {
    AppSettings& s = AppSettings::instance();
    setScene(&m_qtScene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setBackgroundBrush(s.colors.background);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSceneRect(-10000, -10000, 20000, 20000);

    QTransform transform;
    transform.scale(1.0, -1.0);
    setTransform(transform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setDragMode(QGraphicsView::NoDrag);
    setFocusPolicy(Qt::StrongFocus);

    m_inputManager = new InputManager(this, this);

    connect(m_inputManager, &InputManager::undoRequested, [this]() { m_commandStack.undo(); });
    connect(m_inputManager, &InputManager::redoRequested, [this]() { m_commandStack.redo(); });
    connect(m_inputManager, &InputManager::escapePressed, this, &DrawingBoard::escapePressed);
    connect(m_inputManager, &InputManager::statusMessage, this, &DrawingBoard::statusMessage);
    connect(m_inputManager, &InputManager::shortcutModeChanged, this, &DrawingBoard::shortcutModeChanged);
    connect(m_inputManager, &InputManager::toolChangeRequested, this, &DrawingBoard::onToolChangeRequested);

    connect(&m_adapter, &SceneAdapter::selectionChanged, [this]() {
        viewport()->update();
    });
}

void DrawingBoard::wheelEvent(QWheelEvent *event) { m_inputManager->handleWheel(event); }
void DrawingBoard::mousePressEvent(QMouseEvent *event) { m_inputManager->handleMousePress(event); }
void DrawingBoard::mouseMoveEvent(QMouseEvent *event) { m_inputManager->handleMouseMove(event); }
void DrawingBoard::mouseReleaseEvent(QMouseEvent *event) { m_inputManager->handleMouseRelease(event); }
void DrawingBoard::keyPressEvent(QKeyEvent *event) { m_inputManager->handleKeyPress(event); }
void DrawingBoard::keyReleaseEvent(QKeyEvent *event) { m_inputManager->handleKeyRelease(event); }

void DrawingBoard::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawBackground(painter, rect);
    m_grid.drawBackground(painter, rect);
    drawWatermark(painter);
}

void DrawingBoard::drawWatermark(QPainter *painter) const {
    painter->save();
    painter->setTransform(QTransform());

    /*QFont font = painter->font();
    font.setPointSize(72);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(AppSettings::instance().colors.watermark);

    painter->drawText(viewport()->rect(), Qt::AlignCenter, "Στοιχεῖα");*/
    const QImage image(":/resources/logo.png");
    painter->setOpacity(0.1);
    QPoint vc = viewport()->rect().center();
    QSize size = image.size();
    painter->drawImage(vc.x() - size.width() / 2, vc.y() - size.height() / 2, image);
    painter->restore();
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

void DrawingBoard::setGridVisible(const bool visible) {
    m_gridVisible = visible;
    viewport()->update();
}

void DrawingBoard::setGridSpacing(const double spacing) {
    m_gridSpacing = spacing;
    viewport()->update();
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

void DrawingBoard::updateToolType(ToolType type) {
    m_activeToolType = type;
    emit toolChanged(type);
}

void DrawingBoard::applySettings() {
    const auto& s = AppSettings::instance();
    setBackgroundBrush(s.colors.background);
    m_grid.setVisible(s.grid.visible);
    m_grid.setSpacing(s.grid.spacing);
    m_grid.setSnapEnabled(s.grid.snapEnabled);
    m_grid.setAxisColor(s.grid.axisColor);
    m_grid.setGridColor(s.grid.gridColor);
    m_grid.setLabelColor(s.grid.labelColor);
    viewport()->update();
    scene()->update();
    if (m_activeTool)
        m_activeTool->activate();
}

void DrawingBoard::copySelection() {
    m_adapter.copySelection();
}

void DrawingBoard::pasteSelection() {
    const auto& clipboard = m_adapter.clipboard();
    if (!clipboard.empty()) {
        m_commandStack.execute(
                std::make_unique<CopyCommand>(
                    &m_adapter,
                    clipboard,
                    QPointF(50, -50)));
    }
}

void DrawingBoard::onToolChangeRequested(int toolType, int subType) {
    switch (static_cast<ToolType>(toolType)) {
        case ToolType::Select:
            setTool<SelectTool>(ToolType::Select);
            break;
        case ToolType::CreatePoint:
            setTool<CreatePointTool>(ToolType::CreatePoint);
            break;
        case ToolType::CreateLine:
            setTool<CreateLineTool>(ToolType::CreateLine,
                static_cast<LinearObjectType>(subType));
            break;
        case ToolType::CreateRay:
            setTool<CreateLineTool>(ToolType::CreateRay,
                static_cast<LinearObjectType>(subType));
            break;
        case ToolType::CreateSegment:
            setTool<CreateLineTool>(ToolType::CreateSegment,
                static_cast<LinearObjectType>(subType));
            break;
        case ToolType::CreateCircle:
            setTool<CreateCircleTool>(ToolType::CreateCircle);
            break;
        case ToolType::CreateIntersection:
            setTool<CreateIntersectionTool>(ToolType::CreateIntersection);
            break;
        case ToolType::CreateMidpoint:
            setTool<CreateMidpointTool>(ToolType::CreateMidpoint);
            break;
        case ToolType::CreateParallel:
            setTool<CreateParallelTool>(ToolType::CreateParallel);
            break;
        case ToolType::CreatePerpendicular:
            setTool<CreatePerpendicularTool>(ToolType::CreatePerpendicular);
            break;
        case ToolType::CreatePerpendicularFoot:
            setTool<CreatePerpendicularFootTool>(
                ToolType::CreatePerpendicularFoot);
            break;
    }
}

void DrawingBoard::statusMessage(int sbp, const QString &text) {
    emit statusBarTextChanged(static_cast<StatusBarPart>(sbp), text);
}
