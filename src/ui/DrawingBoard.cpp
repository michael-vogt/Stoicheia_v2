#include "DrawingBoard.h"

#include <iostream>
#include <QMouseEvent>
#include <QScrollBar>

#include "constructions/IntersectionSet.h"
#include "geometry/Circle.h"
#include "geometry/LinearObject.h"
#include "geometry/Point.h"

DrawingBoard::DrawingBoard() {
    m_graphicsScene = new QGraphicsScene(this);
    setScene(m_graphicsScene);

    m_graphicsScene->setSceneRect(-10000,-10000,20000,20000);

    updateScene();

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

    QPen pen(Qt::gray);
    painter->setPen(pen);

    const int left = static_cast<int>(std::floor(rect.left())) - (static_cast<int>(std::floor(rect.left())) % m_gridSize);
    const int top = static_cast<int>(std::floor(rect.top())) - (static_cast<int>(std::floor(rect.top())) % m_gridSize);

    QRect intRect = rect.toRect();

    int yPosText = 0;
    if (-top >= rect.height() - m_gridSize) {
        yPosText = top + intRect.height() - m_gridSize;
    } else if (top >= m_gridSize) {
        yPosText = top + m_gridSize;
    }

    for (int x = left; x < intRect.right(); x += m_gridSize) {
        painter->drawLine(x, intRect.top(), x, intRect.bottom());
        std::string str = std::format("{}", x / m_gridSize);
        painter->drawText(x, yPosText, QString::fromStdString(str));
    }

    int xPosText = 0;
    if (-left >= rect.width()-m_gridSize) {
        xPosText = left + intRect.width() - m_gridSize;
    } else if (left >= m_gridSize) {
        xPosText = left + m_gridSize;
    }

    for (int y = top; y < intRect.bottom(); y += m_gridSize) {
        painter->drawLine(intRect.left(), y, intRect.right(), y);
        std::string str = std::format("{}", y / m_gridSize);
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

void DrawingBoard::updateScene() const {
    m_graphicsScene->clear();
    for (auto p : Scene::getInstance().objectsOfType<Point>()) {
        const double x = p->x() * m_gridSize, y = p->y() * m_gridSize;
        m_graphicsScene->addEllipse(x-3, y-3, 6, 6);
    }

    for (auto l : Scene::getInstance().objectsOfType<LinearObject>()) {
        Point *p1 = l->p1(), *p2 = l->p2();
        const double x1 = p1->x() * m_gridSize, y1 = p1->y() * m_gridSize, x2 = p2->x() * m_gridSize, y2 = p2->y() * m_gridSize;
        m_graphicsScene->addLine(x1, y1, x2, y2);
    }

    for (auto c : Scene::getInstance().objectsOfType<Circle>()) {
        Point *center = c->center();
        double radius = c->radius() * m_gridSize;
        double x = center->x() * m_gridSize - radius, y = center->y() * m_gridSize - radius;
        m_graphicsScene->addEllipse(x, y, 2*radius, 2*radius);
    }

    for (auto inter : Scene::getInstance().objectsOfType<IntersectionSet>()) {
        Point* P1 = inter->first(), *P2 = inter->second();

        if (P1->isValid()) {
            m_graphicsScene->addEllipse(P1->x() * m_gridSize - 3, P1->y() * m_gridSize - 3, 6, 6);
        }

        if (P2->isValid()) {
            m_graphicsScene->addEllipse(P2->x() * m_gridSize - 3, P2->y() * m_gridSize - 3, 6, 6);
        }
    }
}
