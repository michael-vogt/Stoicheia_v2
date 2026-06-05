#pragma once
#include <QObject>
#include <QPoint>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include "Enums.h"

class DrawingBoard;
class Tool;

class InputManager : public QObject {
    Q_OBJECT
public:
    explicit InputManager(DrawingBoard* board, QObject* parent = nullptr);

    void setActiveTool(Tool* tool) { m_activeTool = tool; }
    Tool* activeTool() const       { return m_activeTool; }

    // DrawingBoard delegiert alle Events hierher
    void handleMousePress  (QMouseEvent*  event);
    void handleMouseMove   (QMouseEvent*  event);
    void handleMouseRelease(QMouseEvent*  event);
    void handleWheel       (QWheelEvent*  event);
    void handleKeyPress    (QKeyEvent*    event);
    void handleKeyRelease  (QKeyEvent*    event);

    // Shortcut-Modus
    ShortcutMode shortcutMode() const { return m_shortcutMode; }

signals:
    void shortcutModeChanged(ShortcutMode mode);
    void statusMessage(int sbp, const QString& message);
    void escapePressed();
    void toolChangeRequested(int toolType, int subType = 0);
    void undoRequested();
    void redoRequested();

private:
    // Pan
    bool isPanTrigger(QMouseEvent* event) const;
    void startPan   (QMouseEvent* event);
    void updatePan  (QMouseEvent* event);
    void endPan     (QMouseEvent* event);

    // Shortcut-System
    void handleShortcutKey(QKeyEvent* event);
    void setShortcutMode  (ShortcutMode mode);
    void showStatus       (int sbp, const QString& msg);

    // Snap
    void setSnapping(bool snapping);
    bool snapping   () const        { return m_snapping; }

    DrawingBoard* m_board;
    Tool*         m_activeTool   = nullptr;
    ShortcutMode  m_shortcutMode = ShortcutMode::None;

    // Pan-Zustand
    bool   m_panning      = false;
    bool   m_spacePressed = false;
    QPoint m_panStart;

    // Snap-Zustand
    bool m_snapping = false;
};