#pragma once
#include <QCursor>
#include <QMouseEvent>
#include <QKeyEvent>

#include "ui/ToolContext.h"

class Tool : public QObject {
protected:
    ToolContext m_ctx;

public:
    explicit Tool(const ToolContext ctx) : m_ctx(ctx) {}
    virtual ~Tool() = default;

    virtual void activate() {}
    virtual void deactivate() {}

    virtual void mousePressEvent(QMouseEvent* event) { event->ignore(); }
    virtual void mouseMoveEvent(QMouseEvent* event) { event->ignore(); }
    virtual void mouseReleaseEvent(QMouseEvent* event) { event->ignore(); }

    virtual void keyPressEvent(QKeyEvent* event) { event->ignore(); }
    virtual void keyReleaseEvent(QKeyEvent* event) { event->ignore(); }

    virtual QCursor cursor() const { return Qt::ArrowCursor; }
};
