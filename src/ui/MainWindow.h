#pragma once

#include <QMainWindow>

#include "DrawingBoard.h"

class MainWindow : public QMainWindow {
    DrawingBoard* m_drawingBoard;
public:
    explicit MainWindow(const QString& title = "Stoicheia");
};
