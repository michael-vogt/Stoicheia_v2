#include "MainWindow.h"

MainWindow::MainWindow(const QString& title) {
    setWindowTitle(title);
    resize(800, 600);

    m_drawingBoard = new DrawingBoard();
    setCentralWidget(m_drawingBoard);

    showMaximized();
    m_drawingBoard->update();
}
