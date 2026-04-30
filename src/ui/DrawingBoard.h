#pragma once

#include <QGraphicsView>

#include "geometry/Scene.h"

class DrawingBoard : public QGraphicsView {
    QGraphicsScene* m_graphicsScene = nullptr;
    //Scene* m_scene = nullptr;
    bool m_panning = false;
    QPoint m_lastMousePos;
    int m_zoomLevel = 0;
    int m_minZoom = -10;
    int m_maxZoom = 20;
    double m_zoomFactor = 1.15;
    int m_gridSize = 50;

    void zoomIn();
    void zoomOut();

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
public:
    DrawingBoard();
    void resetView();
    [[nodiscard]] int zoomPercent() const;
    void updateScene() const;
};

