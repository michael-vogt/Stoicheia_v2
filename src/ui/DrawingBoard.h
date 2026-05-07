#pragma once

#include <QGraphicsView>

#include "SceneAdapter.h"
#include "geometry/Scene.h"

class DrawingBoard : public QGraphicsView {
    Scene m_geoScene;
    QGraphicsScene m_qtScene;
    SceneAdapter m_adapter;

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
    explicit DrawingBoard(QWidget* parent = nullptr);
    void setGridVisible(bool visible);
    void setGridSpacing(double spacing);
    void resetView();

    Scene* geoScene() { return &m_geoScene; }
    QGraphicsScene* qtScene() { return &m_qtScene; }
    SceneAdapter* adapter() { return &m_adapter; }
};

