#pragma once
#include <QCursor>
#include <QMouseEvent>
#include <QKeyEvent>

#include "ui/ToolContext.h"

class Tool : public QObject {
public:
    explicit Tool(const ToolContext ctx) : m_ctx(ctx) {}
    ~Tool() override = default;

    virtual void activate() {}
    virtual void deactivate() {}

    virtual void mousePressEvent(QMouseEvent* event) { event->ignore(); }
    virtual void mouseMoveEvent(QMouseEvent* event) { event->ignore(); }
    virtual void mouseReleaseEvent(QMouseEvent* event) { event->ignore(); }

    virtual void keyPressEvent(QKeyEvent* event) { event->ignore(); }
    virtual void keyReleaseEvent(QKeyEvent* event) { event->ignore(); }

    [[nodiscard]] virtual auto cursor() const -> QCursor { return Qt::ArrowCursor; }

protected:
    ToolContext m_ctx;
};
