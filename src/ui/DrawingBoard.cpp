#include "DrawingBoard.h"

#include <QScrollBar>
#include <QTimer>
#include <qevent.h>

#include "../Constants.h"
#include "../GuiConstants.h"
#include "Structs.h"
#include "commands/CopyCommand.h"
#include "dialogs/AppSettings.h"
#include "tools/CreatePointTool.h"
#include "tools/CreateLineTool.h"
#include "tools/CreateCircleTool.h"
#include "tools/CreateIntersectionTool.h"
#include "tools/CreateMidpointTool.h"
#include "tools/CreateParallelTool.h"
#include "tools/CreatePerpendicularTool.h"
#include "tools/CreatePerpendicularFootTool.h"
#include "tools/SelectTool.h"


using namespace Constants;
using namespace GuiConstants;

DrawingBoard::DrawingBoard(QWidget *parent)
: QGraphicsView(parent), m_adapter(&m_geoScene, &m_qtScene), m_activeTool(nullptr), m_activeToolType(ToolType::Select) {
    AppSettings& settings = AppSettings::instance();
    setScene(&m_qtScene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setBackgroundBrush(settings.colors.background);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSceneRect(
        -UiMetricsConstants::SCENERECT_UNIT, -UiMetricsConstants::SCENERECT_UNIT,
        2 * UiMetricsConstants::SCENERECT_UNIT, UiMetricsConstants::SCENERECT_UNIT * 2);

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
    connect(m_inputManager, &InputManager::zoomChanged, this, [this](double zoomPercent) {
        showStatusRight(tr("Zoom: %1%").arg(qRound(zoomPercent)));
    });

    connect(&m_adapter, &SceneAdapter::selectionChanged, [this]() {
        viewport()->update();
    });
}

void DrawingBoard::updateToolType(ToolType type) {
    m_activeToolType = type;
    emit toolChanged(type);
}

void DrawingBoard::setGridVisible(const bool visible) {
    m_gridVisible = visible;
    viewport()->update();
}

void DrawingBoard::setGridSpacing(const double spacing) {
    m_gridSpacing = spacing;
    viewport()->update();
}

void DrawingBoard::resetView() {
    setTransformationAnchor(QGraphicsView::NoAnchor);
    QTransform trans = transform();
    double scaleX = trans.m11(); // in case of rotation and/or shear: std::sqrt(t.m11() * t.m11() + t.m21() * t.m21());
    double scaleY = trans.m22(); // in case of rotation and/or shear: std::sqrt(t.m22() * t.m22() + t.m12() * t.m12());
    centerOn(0, 0);
    scale(1.0 / scaleX, -1.0 / scaleY);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
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
                    QPointF(UiMetricsConstants::PASTE_OFFSET, -UiMetricsConstants::PASTE_OFFSET)));
    }
}

void DrawingBoard::applySettings() {
    const auto& settings = AppSettings::instance();
    setBackgroundBrush(settings.colors.background);
    m_grid.setVisible(settings.grid.visible);
    m_grid.setSpacing(settings.grid.spacing);
    m_grid.setSnapEnabled(settings.grid.snapEnabled);
    m_grid.setAxisColor(settings.grid.axisColor);
    m_grid.setGridColor(settings.grid.gridColor);
    m_grid.setLabelColor(settings.grid.labelColor);
    viewport()->update();
    scene()->update();
    if (m_activeTool) {
        m_activeTool->activate();
    }
}

void DrawingBoard::onToolChangeRequested(ToolTypePair toolTypePair) {
    switch (static_cast<ToolType>(toolTypePair.toolType)) {
        case ToolType::Select:
            setTool<SelectTool>(ToolType::Select);
            break;
        case ToolType::CreatePoint:
            setTool<CreatePointTool>(ToolType::CreatePoint);
            break;
        case ToolType::CreateLine:
            setTool<CreateLineTool>(ToolType::CreateLine,
                static_cast<LinearObjectType>(toolTypePair.subType));
            break;
        case ToolType::CreateRay:
            setTool<CreateLineTool>(ToolType::CreateRay,
                static_cast<LinearObjectType>(toolTypePair.subType));
            break;
        case ToolType::CreateSegment:
            setTool<CreateLineTool>(ToolType::CreateSegment,
                static_cast<LinearObjectType>(toolTypePair.subType));
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

void DrawingBoard::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawBackground(painter, rect);
    m_grid.drawBackground(painter, rect);
    drawWatermark(painter);
}

void DrawingBoard::drawForeground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawForeground(painter, rect);
    if (!m_grid.isVisible()) {
        return;
    }

    painter->save();
    painter->setTransform(QTransform());

    auto toViewport = [this](QPointF point) -> QPointF {
        return viewport()->mapFrom(this, mapFromScene(point));
    };

    m_grid.drawLabels(painter, toViewport, {.width=viewport()->width(), .height=viewport()->height()});
    painter->restore();
}

void DrawingBoard::keyPressEvent(QKeyEvent *event) { m_inputManager->handleKeyPress(event); }
void DrawingBoard::keyReleaseEvent(QKeyEvent *event) { m_inputManager->handleKeyRelease(event); }
void DrawingBoard::mousePressEvent(QMouseEvent *event) { m_inputManager->handleMousePress(event); }
void DrawingBoard::mouseMoveEvent(QMouseEvent *event) { m_inputManager->handleMouseMove(event); }
void DrawingBoard::mouseReleaseEvent(QMouseEvent *event) { m_inputManager->handleMouseRelease(event); }
void DrawingBoard::mouseDoubleClickEvent(QMouseEvent *event) { m_inputManager->handleMouseDoubleClick(event); }
void DrawingBoard::wheelEvent(QWheelEvent *event) { m_inputManager->handleWheel(event); }

void DrawingBoard::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    if (!m_initialCenterDone) {
        m_initialCenterDone = true;
        const QPointF origin = mapFromScene(QPointF(0, 0));
        const auto center = QPointF(viewport()->rect().center());
        const QPointF delta = origin - center;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + qRound(delta.x()));
        verticalScrollBar()->setValue(verticalScrollBar()->value() + qRound(delta.y()));
    }
}

void DrawingBoard::moveEvent(QMoveEvent *event) {
    QGraphicsView::moveEvent(event);
    viewport()->update();
}

void DrawingBoard::resizeEvent(QResizeEvent *event) {
    if (!m_initialCenterDone) {
        QGraphicsView::resizeEvent(event);
        viewport()->update();
        return;
    }

    int hBefore = horizontalScrollBar()->value();
    int vBefore = verticalScrollBar()->value();

    QGraphicsView::resizeEvent(event);

    QSize delta = event->size() - event->oldSize();
    horizontalScrollBar()->setValue(hBefore - delta.width() / 2);
    verticalScrollBar()->setValue(vBefore - delta.height() / 2);

    viewport()->update();
}

void DrawingBoard::drawWatermark(QPainter *painter) const {
    painter->save();
    painter->setTransform(QTransform());

    const QImage image(":/resources/logo.png");
    painter->setOpacity(Colors::WATERMARK_OPACITY);
    const QRectF vr = viewport()->rect();
    const QPointF vc = vr.center();
    const QSizeF size = image.size() / image.devicePixelRatio();
    painter->drawImage(QPointF(vc.x() - size.width() / 2, vc.y() - size.height() / 2), image);
    painter->restore();
}
