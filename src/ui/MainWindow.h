#pragma once

#include <QMainWindow>

#include "DrawingBoard.h"
#include "SceneAdapter.h"

class MainWindow : public QMainWindow {
    DrawingBoard* m_drawingBoard = nullptr;

    void setupMenu();
    void setupStatusBar() const;

public:
    explicit MainWindow(const QString& title = "Stoicheia", QWidget* parent = nullptr);

    DrawingBoard* drawingBoard() const { return m_drawingBoard; }
};
