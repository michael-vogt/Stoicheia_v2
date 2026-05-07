#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(const QString& title, QWidget* parent) : QMainWindow(parent), m_adapter(&m_geoScene, &m_qtScene) {
    setWindowTitle(title);

    m_drawingBoard = new DrawingBoard(&m_qtScene, this);
    setCentralWidget(m_drawingBoard);

    setupMenu();
    setupStatusBar();
}

void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu(tr("Datei"));

    QAction* quitAction = fileMenu->addAction(tr("Beenden"));
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    QMenu* viewMenu = menuBar()->addMenu(tr("Ansicht"));

    QAction* gridAction = viewMenu->addAction(tr("Raster"));
    gridAction->setCheckable(true);
    gridAction->setChecked(true);
    connect(gridAction, &QAction::toggled,
            m_drawingBoard, &DrawingBoard::setGridVisible);
}

void MainWindow::setupStatusBar() const {
    statusBar()->showMessage(tr("Bereit"));
}
