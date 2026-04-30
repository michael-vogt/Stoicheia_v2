#include "DrawingBoard.h"

#include <QMouseEvent>
#include <QScrollBar>

DrawingBoard::DrawingBoard() {
    m_graphicsScene = new QGraphicsScene(this);
    setScene(m_graphicsScene);

    m_graphicsScene->setSceneRect(-10000,-10000,20000,20000);

    /*m_graphicsScene->addLine(-200, 0, 200, 0);
    m_graphicsScene->addLine(0, -200, 0, 200);
    m_graphicsScene->addEllipse(-50, -50, 100, 100);*/

    setRenderHint(QPainter::Antialiasing, true);

    setDragMode(NoDrag);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    centerOn(0,0);
}

void DrawingBoard::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawBackground(painter, rect);

    painter->fillRect(rect, Qt::lightGray);

    QRect vpRect = viewport()->rect();

    constexpr int gridSize = 50;

    QPen pen(Qt::gray);
    painter->setPen(pen);

    const int left = static_cast<int>(std::floor(rect.left())) - (static_cast<int>(std::floor(rect.left())) % gridSize);
    const int top = static_cast<int>(std::floor(rect.top())) - (static_cast<int>(std::floor(rect.top())) % gridSize);

    QRect intRect = rect.toRect();

    int yPosText = 0;
    if (-top >= rect.height() - gridSize) {
        yPosText = top + intRect.height() - gridSize;
    } else if (top >= gridSize) {
        yPosText = top + gridSize;
    }

    for (int x = left; x < intRect.right(); x += gridSize) {
        painter->drawLine(x, intRect.top(), x, intRect.bottom());
        std::string str = std::format("{}", x / gridSize);
        painter->drawText(x, yPosText, QString::fromStdString(str));
    }

    int xPosText = 0;
    if (-left >= rect.width()-gridSize) {
        xPosText = left + intRect.width() - gridSize;
    } else if (left >= gridSize) {
        xPosText = left + gridSize;
    }

    for (int y = top; y < intRect.bottom(); y += gridSize) {
        painter->drawLine(intRect.left(), y, intRect.right(), y);
        std::string str = std::format("{}", y / gridSize);
        painter->drawText(xPosText, y, QString::fromStdString(str));
    }

    // Achsen hervorheben
    painter->setPen(QPen(Qt::red, 0));
    painter->drawLine(0, intRect.top(), 0, intRect.bottom());

    painter->setPen(QPen(Qt::green, 0));
    painter->drawLine(intRect.left(), 0, intRect.right(), 0);
}

void DrawingBoard::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_R) {
        resetView();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}

void DrawingBoard::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void DrawingBoard::mouseMoveEvent(QMouseEvent *event) {
    if (m_panning) {
        QPoint delta = event->pos() - m_lastMousePos;

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        m_lastMousePos = event->pos();
        event->accept();
        viewport()->update();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void DrawingBoard::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        viewport()->update();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void DrawingBoard::wheelEvent(QWheelEvent *event) {
    constexpr double zoomFactor = 1.15;

    if (event->angleDelta().y() > 0)
        scale(zoomFactor, zoomFactor);
    else
        scale(1.0 / zoomFactor, 1.0 / zoomFactor);
}

void DrawingBoard::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    setBackgroundBrush(Qt::lightGray);
    //translate(500, 0);
}

void DrawingBoard::resetView() {
    resetTransform();
    m_zoomLevel = 0;
    centerOn(0,0);
}

void DrawingBoard::zoomIn() {
    if (m_zoomLevel >= m_maxZoom)
        return;

    scale(m_zoomFactor, m_zoomFactor);
    ++m_zoomLevel;
}

void DrawingBoard::zoomOut() {
    if (m_zoomLevel <= m_minZoom)
        return;

    scale(1.0 / m_zoomFactor, 1.0 / m_zoomFactor);
    --m_zoomLevel;
}

int DrawingBoard::zoomPercent() const {
    return static_cast<int>(std::pow(m_zoomFactor, m_zoomLevel) * 100.0);
}
