#include "MainWindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QLineEdit>

#include "tools/CreateCircleTool.h"
#include "tools/CreateLineTool.h"
#include "tools/CreatePointTool.h"
#include "tools/SelectTool.h"

MainWindow::MainWindow(const QString& title, QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(title);

    m_drawingBoard = new DrawingBoard(this);
    setCentralWidget(m_drawingBoard);

    setupToolBar();
    setupMenu();
    setupStatusBar();

    connect(m_drawingBoard->commandStack(), &CommandStack::changed, this, &MainWindow::updateUndoRedo);
    connect(m_drawingBoard, &DrawingBoard::statusMessageChanged, statusBar(), &QStatusBar::showMessage);
    connect(m_drawingBoard, &DrawingBoard::toolChanged, this, [this](ToolType type) {
        const bool isDrawing = (type == ToolType::CreateLine);
        m_lineAction->setShortcut(isDrawing ? QKeySequence() : QKeySequence(Qt::Key_L));
    });

    // Standard-Tool: Auswählen
    m_drawingBoard->setTool<SelectTool>(ToolType::Select);
}

void MainWindow::setupToolBar() {
    m_toolbar = addToolBar(tr("Werkzeuge"));
    m_toolbar->setMovable(false);

    auto* selectAction = m_toolbar->addAction(tr("Auswählen"));
    selectAction->setCheckable(true);
    selectAction->setChecked(true);
    connect(m_drawingBoard, &DrawingBoard::escapePressed, selectAction, &QAction::trigger);
    connect(selectAction, &QAction::triggered, [this, selectAction]() {
        m_drawingBoard->setTool<SelectTool>(ToolType::Select);
        toggleTools(selectAction);
    });

    auto* pointAction = m_toolbar->addAction(tr("Punkt"));
    pointAction->setCheckable(true);
    pointAction->setShortcut(Qt::Key_P);
    connect(pointAction, &QAction::triggered, [this, pointAction]() {
        m_drawingBoard->setTool<CreatePointTool>(ToolType::CreatePoint);
        toggleTools(pointAction);
    });


    m_lineAction = m_toolbar->addAction(tr("Linie"));
    m_lineAction->setCheckable(true);
    m_lineAction->setShortcut(Qt::Key_L);
    connect(m_lineAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreateLineTool>(ToolType::CreateLine, LinearObjectType::Line);
        toggleTools(m_lineAction);
    });

    auto* circleAction = m_toolbar->addAction(tr("Kreis"));
    circleAction->setCheckable(true);
    circleAction->setShortcut(Qt::Key_C);
    connect(circleAction, &QAction::triggered, [this, circleAction]() {
        m_drawingBoard->setTool<CreateCircleTool>(ToolType::CreateCircle);
        toggleTools(circleAction);
    });
}

void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu(tr("Datei"));

    QAction* quitAction = fileMenu->addAction(tr("Beenden"));
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);


    QMenu* editMenu = menuBar()->addMenu(tr("Bearbeiten"));

    m_undoAction = editMenu->addAction(tr("Rückgängig"));
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setEnabled(false);
    connect(m_undoAction, &QAction::triggered, [this]() {
        m_drawingBoard->commandStack()->undo();
        updateUndoRedo();
    });

    m_redoAction = editMenu->addAction(tr("Wiederholen"));
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setEnabled(false);
    connect(m_redoAction, &QAction::triggered, [this]() {
        m_drawingBoard->commandStack()->redo();
        updateUndoRedo();
    });


    QMenu* viewMenu = menuBar()->addMenu(tr("Ansicht"));

    QAction* gridAction = viewMenu->addAction(tr("Raster"));
    gridAction->setCheckable(true);
    gridAction->setChecked(true);
    connect(gridAction, &QAction::toggled,
            m_drawingBoard, &DrawingBoard::setGridVisible);
}

void MainWindow::setupStatusBar() const {
    //statusBar()->showMessage(tr("Bereit"));
}

void MainWindow::updateUndoRedo() const {
    m_undoAction->setEnabled(m_drawingBoard->commandStack()->canUndo());
    m_redoAction->setEnabled(m_drawingBoard->commandStack()->canRedo());

    QString undoDesc = m_drawingBoard->commandStack()->nextUndoDescription();
    QString redoDesc = m_drawingBoard->commandStack()->nextRedoDescription();

    m_undoAction->setText(undoDesc.isEmpty() ? tr("Rückgängig") : tr("Rückgängig: ") + undoDesc);
    m_redoAction->setText(redoDesc.isEmpty() ? tr("Wiederholen") : tr("Wiederholen: ") + redoDesc);
}

void MainWindow::toggleTools(const QAction *selectedAction) const {
    for (auto* action : m_toolbar->actions()) {
        if (action == selectedAction) {
            action->setChecked(true);
        } else {
            action->setChecked(false);
        }
    }
}
