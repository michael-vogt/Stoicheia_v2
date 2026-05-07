#pragma once

#include <QGraphicsView>

#include "geometry/Scene.h"

class DrawingBoard : public QGraphicsView {
    bool m_panning = false;
    bool m_spacePressed = false;
    QPoint m_panStart;


    bool m_gridVisible = true;
    double m_gridSpacing = 50.0;

    void drawGrid(QPainter* painter, const QRectF& rect) const;

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
public:
    explicit DrawingBoard(QGraphicsScene* scene, QWidget* parent = nullptr);
    void setGridVisible(bool visible);
    void setGridSpacing(double spacing);
    void resetView();
};

