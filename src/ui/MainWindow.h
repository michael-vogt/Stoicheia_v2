#pragma once

#include <QMainWindow>

#include "DrawingBoard.h"
#include "SceneAdapter.h"

class MainWindow : public QMainWindow {
    Scene m_geoScene;
    QGraphicsScene m_qtScene;
    SceneAdapter m_adapter;
    DrawingBoard* m_drawingBoard = nullptr;

    void setupMenu();
    void setupStatusBar() const;

public:
    explicit MainWindow(const QString& title = "Stoicheia", QWidget* parent = nullptr);

    Scene* geoScene() { return &m_geoScene; }
    QGraphicsScene* qtScene() { return &m_qtScene; }
    SceneAdapter* adapter() { return &m_adapter; }
    DrawingBoard* drawingBoard() { return m_drawingBoard; }
};
