#include "DrawingBoard.h"

#include <iostream>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStatusBar>
#include <QTimer>
#include <QShortcut>

#include "MainWindow.h"
#include "geometry/Point.h"

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
    /*if (m_gridVisible) {
        drawGrid(painter, rect);
    }*/
}

/*void DrawingBoard::drawGrid(QPainter* painter, const QRectF& rect) const {
    // Axes
    QPen axisPen(QColor(160,160,160), 1.5);
    painter->setPen(axisPen);
    painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0));
    painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom()));

    // grid lines
    QPen gridPen(QColor(220,220,220), 0.5);
    painter->setPen(gridPen);

    double left = std::floor(rect.left() / m_gridSpacing) * m_gridSpacing;
    double top = std::floor(rect.top() / m_gridSpacing) * m_gridSpacing;
    double right = rect.right();
    double bottom = rect.bottom();

    for (double x = left; x <= right; x += m_gridSpacing) {
        if (std::abs(x) > 0.1)
            painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    }

    for (double y = top; y <= bottom; y += m_gridSpacing) {
        if (std::abs(y) > 0.1)
            painter->drawLine(QPointF(left, y), QPointF(right, y));
    }
}*/

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

    /*QFont font = painter->font();
    font.setPointSize(8);
    painter->setFont(font);
    painter->setPen(QColor(120,120,120));

    const int margin = 4;
    const int tickSize = 4;

    double left = std::floor(rect.left() / m_gridSpacing) * m_gridSpacing;
    double top = std::floor(rect.top() / m_gridSpacing) * m_gridSpacing;
    double right = rect.right();
    double bottom = rect.bottom();

    QPointF originView = viewport()->mapFrom(this, mapFromScene(QPointF(0, 0))); // mapFromScene(QPointF(0,0));
    int ox = static_cast<int>(originView.x());
    int oy = static_cast<int>(originView.y());

    int labelY = static_cast<int>(std::clamp(
        static_cast<double>(oy + margin + 12),
        static_cast<double>(margin + 12),
        static_cast<double>(height() - margin)
        ));

    for (double x = left; x <= right; x += m_gridSpacing) {
        if (std::abs(x) < 0.1) continue;
        int px = static_cast<int>(mapFromScene(QPointF(x, 0)).x());
        painter->drawLine(px, oy - tickSize, px, oy + tickSize);
        QString label = QString::number(static_cast<int>(std::round(x)));
        painter->drawText(QRect(px - 20, labelY - 12, 40, 14), Qt::AlignHCenter, label);
    }

    int labelX = static_cast<int>(std::clamp(
        static_cast<double>(ox + margin),
        static_cast<double>(margin),
        static_cast<double>(width() - 40 - margin)
        ));

    for (double y = top; y <= bottom; y += m_gridSpacing) {
        if (std::abs(y) < 0.1) continue;
        int py = static_cast<int>(mapFromScene(QPointF(0, y)).y());
        painter->drawLine(ox - tickSize, py, ox + tickSize, py);
        QString label = QString::number(static_cast<int>(std::round(y)));
        painter->drawText(QRect(labelX, py-7,38,14), Qt::AlignLeft | Qt::AlignVCenter, label);
    }

    painter->drawText(QRect(ox + margin, oy + margin, 20, 14), Qt::AlignLeft, "0");

    painter->restore();*/
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

    if (event->key() == Qt::Key_M && !event->isAutoRepeat()) {
        for (const auto& map = m_adapter.geoGraphicsItems(); const auto &key: map | std::views::keys) {
            if (key && typeid(*key) == typeid(Point)) {
                auto* p = dynamic_cast<Point*>(key);
                p->moveTo(p->x() - 100, p->y());
                break;
            }
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_R && !event->isAutoRepeat()) {
        resetView();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_NumberSign && !event->isAutoRepeat()) {
        setGridVisible(!m_gridVisible);
        event->accept();
        return;
    }

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
