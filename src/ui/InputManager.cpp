#include "InputManager.h"
#include "DrawingBoard.h"
#include "tools/Tool.h"
#include "tools/CreatePointTool.h"
#include "tools/CreatePerpendicularFootTool.h"
#include "LinearObjectType.h"
#include <QScrollBar>
#include <QShortcut>

InputManager::InputManager(DrawingBoard* board, QObject* parent)
    : QObject(parent), m_board(board)
{
    // Escape-Shortcut direkt auf dem DrawingBoard – greift auch während Drag
    auto* escShortcut = new QShortcut(Qt::Key_Escape, m_board); // NOLINT
    escShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(escShortcut, &QShortcut::activated, [this]() {
        // 1. Shortcut-Modus abbrechen
        if (m_shortcutMode != ShortcutMode::None) {
            setShortcutMode(ShortcutMode::None);
            if (m_activeTool)
                m_activeTool->activate();
            return;
        }
        // 2. Tool fragen
        if (m_activeTool) {
            QKeyEvent escEvent(QEvent::KeyPress,
                               Qt::Key_Escape, Qt::NoModifier);
            m_activeTool->keyPressEvent(&escEvent);
            if (escEvent.isAccepted()) return;
        }
        // 3. Zum SelectTool wechseln
        emit escapePressed();
    });
}

// ── Pan ──────────────────────────────────────────────────────────────────────

bool InputManager::isPanTrigger(QMouseEvent* event) const {
    return event->button() == Qt::MiddleButton ||
           (event->button() == Qt::LeftButton && m_spacePressed);
}

void InputManager::startPan(QMouseEvent* event) {
    m_panning  = true;
    m_panStart = event->pos();
    m_board->viewport()->setCursor(Qt::ClosedHandCursor);
    showStatus(std::to_underlying(StatusBarPart::Right), tr("Panning"));
    event->accept();
}

void InputManager::updatePan(QMouseEvent* event) {
    QPoint delta = event->pos() - m_panStart;
    m_panStart   = event->pos();
    m_board->horizontalScrollBar()->setValue(
        m_board->horizontalScrollBar()->value() - delta.x());
    m_board->verticalScrollBar()->setValue(
        m_board->verticalScrollBar()->value() - delta.y());
    event->accept();
}

void InputManager::endPan(QMouseEvent* event) {
    m_panning = false;
    m_board->viewport()->setCursor(
        m_spacePressed ? Qt::OpenHandCursor : Qt::ArrowCursor);
    showStatus(std::to_underlying(StatusBarPart::Right), "");
    event->accept();
}

// ── Mouse Events ─────────────────────────────────────────────────────────────

void InputManager::handleMousePress(QMouseEvent* event) {
    if (isPanTrigger(event)) {
        startPan(event);
        return;
    }
    if (m_activeTool) {
        m_activeTool->mousePressEvent(event);
        if (event->isAccepted()) return;
    }
    event->ignore();
}

void InputManager::handleMouseMove(QMouseEvent* event) {
    if (m_panning) {
        updatePan(event);
        return;
    }
    if (m_activeTool) {
        m_activeTool->mouseMoveEvent(event);
        if (event->isAccepted()) return;
    }
    event->ignore();
}

void InputManager::handleMouseRelease(QMouseEvent* event) {
    if (m_panning) {
        endPan(event);
        return;
    }
    if (m_activeTool) {
        m_activeTool->mouseReleaseEvent(event);
        if (event->isAccepted()) return;
    }
    event->ignore();
}

void InputManager::handleWheel(QWheelEvent* event) {
    if (m_spacePressed) { event->ignore(); return; }
    double factor = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
    m_board->scale(factor, factor);
    event->accept();
}

// ── Key Events ───────────────────────────────────────────────────────────────

void InputManager::handleKeyPress(QKeyEvent* event) {
    // Aktiver Shortcut-Modus hat Vorrang
    if (m_shortcutMode != ShortcutMode::None) {
        handleShortcutKey(event);
        if (event->isAccepted()) return;
    }

    // Tool zuerst
    if (m_activeTool) {
        m_activeTool->keyPressEvent(event);
        if (event->isAccepted()) return;
    }

    // Undo/Redo
    if (event->matches(QKeySequence::Undo)) {
        emit undoRequested();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Redo)) {
        emit redoRequested();
        event->accept();
        return;
    }

    // Ctrl+C / Ctrl+V ans Tool weiterleiten
    if (event->modifiers() & Qt::ControlModifier) {
        if (m_activeTool) {
            m_activeTool->keyPressEvent(event);
            if (event->isAccepted()) return;
        }
    }

    // Space → Pan-Modus
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        m_spacePressed = true;
        m_board->viewport()->setCursor(Qt::OpenHandCursor);
        event->accept();
        return;
    }

    // Reset view
    if (event->key() == Qt::Key_R && !event->isAutoRepeat()) {
        m_board->resetView();
        event->accept();
        return;
    }

    // Alt → Snapping
    if (event->key() == Qt::Key_Alt || event->modifiers() & Qt::AltModifier) {
        setSnapping(true);
        event->accept();
        return;
    }

    // G/K → Shortcut-Modus
    handleShortcutKey(event);
}

void InputManager::handleKeyRelease(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        m_spacePressed = false;
        if (!m_panning)
            m_board->viewport()->setCursor(
                m_activeTool ? m_activeTool->cursor() : Qt::ArrowCursor);
        event->accept();
        return;
    }
    if (m_activeTool) {
        m_activeTool->keyReleaseEvent(event);
        if (event->isAccepted()) return;
    }

    if (event->key() == Qt::Key_Alt || event->modifiers() & Qt::AltModifier) {
        setSnapping(false);
        event->accept();
        return;
    }

    event->ignore();
}

// ── Shortcut-System ───────────────────────────────────────────────────────────

void InputManager::showStatus(int sbp, const QString& msg) {
    emit statusMessage(sbp, msg);
}

void InputManager::setShortcutMode(ShortcutMode mode) {
    m_shortcutMode = mode;
    emit shortcutModeChanged(mode);

    constexpr int sbLeft = std::to_underlying(StatusBarPart::Left);

    switch (mode) {
        case ShortcutMode::None:
            break;
        case ShortcutMode::Geometry:
            showStatus(sbLeft, tr("Geometrie: [P] Punkt  [L] Gerade  "
                          "[R] Halbgerade  [S] Strecke  [C] Kreis  "
                          "[Esc] Abbrechen"));
            break;
        case ShortcutMode::Construction:
            showStatus(sbLeft, tr("Konstruktion: [S] Schnittpunkt  [M] Mittelpunkt  "
                          "[P] Parallele  [E] Senkrechte  [L] Lotfußpunkt  "
                          "[Esc] Abbrechen"));
            break;
    }
}

void InputManager::handleShortcutKey(QKeyEvent* event) {
    if (m_shortcutMode == ShortcutMode::None) {
        switch (event->key()) {
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
        m_activeTool->activate();
        event->accept();
        return;
    }

    if (m_shortcutMode == ShortcutMode::Geometry) {
        int subType = -1;
        int toolType = -1;
        switch (event->key()) {
            case Qt::Key_P:
                toolType = static_cast<int>(ToolType::CreatePoint);
                break;
            case Qt::Key_L:
                toolType = static_cast<int>(ToolType::CreateLine);
                subType  = static_cast<int>(LinearObjectType::Line);
                break;
            case Qt::Key_R:
                toolType = static_cast<int>(ToolType::CreateRay);
                subType  = static_cast<int>(LinearObjectType::Ray);
                break;
            case Qt::Key_S:
                toolType = static_cast<int>(ToolType::CreateSegment);
                subType  = static_cast<int>(LinearObjectType::Segment);
                break;
            case Qt::Key_C:
                toolType = static_cast<int>(ToolType::CreateCircle);
                break;
            default:
                event->ignore();
                return;
        }
        setShortcutMode(ShortcutMode::None);
        emit toolChangeRequested(toolType, subType);
        event->accept();
        return;
    }

    if (m_shortcutMode == ShortcutMode::Construction) {
        int toolType = -1;
        switch (event->key()) {
            case Qt::Key_S:
                toolType = static_cast<int>(ToolType::CreateIntersection);
                break;
            case Qt::Key_M:
                toolType = static_cast<int>(ToolType::CreateMidpoint);
                break;
            case Qt::Key_P:
                toolType = static_cast<int>(ToolType::CreateParallel);
                break;
            case Qt::Key_E:
                toolType = static_cast<int>(ToolType::CreatePerpendicular);
                break;
            case Qt::Key_L:
                toolType = static_cast<int>(ToolType::CreatePerpendicularFoot);
                break;
            default:
                event->ignore();
                return;
        }
        setShortcutMode(ShortcutMode::None);
        emit toolChangeRequested(toolType);
        event->accept();
        return;
    }
}

void InputManager::setSnapping(const bool snapping) {
    m_snapping = snapping;
    showStatus(std::to_underlying(StatusBarPart::Right), snapping ? tr("Snapping") : "");
}
