#include "MainWindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaEnum>

#include "tools/CreateCircleTool.h"
#include "tools/CreateIntersectionTool.h"
#include "tools/CreateLineTool.h"
#include "tools/CreateMidpointTool.h"
#include "tools/CreateParallelTool.h"
#include "tools/CreatePerpendicularFootTool.h"
#include "tools/CreatePerpendicularTool.h"
#include "tools/CreatePointTool.h"
#include "tools/SelectTool.h"
#include <magic_enum/magic_enum.hpp>

MainWindow::MainWindow(const QString& title, QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(title);

    m_drawingBoard = new DrawingBoard(this);
    setCentralWidget(m_drawingBoard);

    setupToolBar();
    setupMenu();
    setupStatusBar();

    connect(m_drawingBoard->commandStack(), &CommandStack::changed, this, &MainWindow::updateUndoRedo);
    //connect(m_drawingBoard, &DrawingBoard::statusMessageChanged, statusBar(), &QStatusBar::showMessage);
    connect(m_drawingBoard, &DrawingBoard::statusMessageChanged, [this](const QString& str) {
        statusBar()->showMessage(str);
        if (str.isEmpty()) {
            QMessageBox::information(this, "", "leer");
        }
    });
    connect(m_drawingBoard, &DrawingBoard::toolChanged, this, [this](ToolType type) {
        const bool isDrawing = (type == ToolType::CreateLine);
        m_lineAction->setShortcut(isDrawing ? QKeySequence() : QKeySequence(Qt::Key_L));
        //QString name = QString::fromStdString(std::string(magic_enum::enum_name(type)));
        //QMessageBox::information(this, "Info", name);
        checkTool(type);
        m_drawingBoard->activeTool()->activate();
    });

    // Standard-Tool: Auswählen
    m_drawingBoard->setTool<SelectTool>(ToolType::Select);
}

void MainWindow::setupToolBar() {
    m_toolbar = addToolBar(tr("Werkzeuge"));
    m_toolbar->setMovable(false);

    m_selectAction = m_toolbar->addAction(tr("Auswählen"));
    m_selectAction->setCheckable(true);
    m_selectAction->setChecked(true);
    connect(m_drawingBoard, &DrawingBoard::escapePressed, m_selectAction, &QAction::trigger);
    connect(m_selectAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<SelectTool>(ToolType::Select);
        toggleTools(m_selectAction);
    });

    m_pointAction = m_toolbar->addAction(tr("Punkt"));
    m_pointAction->setCheckable(true);
    m_pointAction->setShortcut(Qt::Key_P);
    connect(m_pointAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreatePointTool>(ToolType::CreatePoint);
        toggleTools(m_pointAction);
    });


    m_lineAction = m_toolbar->addAction(tr("Linie"));
    m_lineAction->setCheckable(true);
    m_lineAction->setShortcut(Qt::Key_L);
    connect(m_lineAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreateLineTool>(ToolType::CreateLine, LinearObjectType::Line);
        toggleTools(m_lineAction);
    });

    m_circleAction = m_toolbar->addAction(tr("Kreis"));
    m_circleAction->setCheckable(true);
    m_circleAction->setShortcut(Qt::Key_C);
    connect(m_circleAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreateCircleTool>(ToolType::CreateCircle);
        toggleTools(m_circleAction);
    });

    m_intersectionAction = m_toolbar->addAction(tr("Schnittpunkt"));
    m_intersectionAction->setCheckable(true);
    connect(m_intersectionAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreateIntersectionTool>(ToolType::CreateIntersection);
        toggleTools(m_intersectionAction);
    });

    m_midpointAction = m_toolbar->addAction(tr("Mittelpunkt"));
    m_midpointAction->setCheckable(true);
    connect(m_midpointAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreateMidpointTool>(ToolType::CreateMidpoint);
        toggleTools(m_midpointAction);
    });

    m_parallelAction = m_toolbar->addAction(tr("Parallele"));
    m_parallelAction->setCheckable(true);
    connect(m_parallelAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreateParallelTool>(ToolType::CreateParallel);
        toggleTools(m_parallelAction);
    });
    

    m_perpendicularAction = m_toolbar->addAction(tr("Senkrechte"));
    m_perpendicularAction->setCheckable(true);
    connect(m_perpendicularAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreatePerpendicularTool>(ToolType::CreatePerpendicular);
        toggleTools(m_perpendicularAction);
    });
    

    m_perpendicularFootAction = m_toolbar->addAction(tr("Lotfußpunkt"));
    m_perpendicularFootAction->setCheckable(true);
    connect(m_perpendicularFootAction, &QAction::triggered, [this]() {
        m_drawingBoard->setTool<CreatePerpendicularFootTool>(ToolType::CreatePerpendicularFoot);
        toggleTools(m_perpendicularFootAction);
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

    QAction* snapAction = viewMenu->addAction(tr("Immer einrasten"));
    snapAction->setCheckable(true);
    snapAction->setChecked(false);
    connect(snapAction, &QAction::toggled, [this](bool on) {
        m_drawingBoard->grid()->setSnapEnabled(on);
    });

    QAction* gridAction = viewMenu->addAction(tr("Raster"));
    gridAction->setCheckable(true);
    gridAction->setChecked(true);
    connect(gridAction, &QAction::toggled, [this](bool on) {
        m_drawingBoard->grid()->setVisible(on);
        m_drawingBoard->viewport()->update();
    });
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
    /*for (auto* action : m_toolbar->actions()) {
        if (action == selectedAction) {
            action->setChecked(true);
        } else {
            action->setChecked(false);
        }
    }*/
}

void MainWindow::checkTool(ToolType type) {
    for (auto* action : m_toolbar->actions())
        action->setChecked(false);
    
        switch (type) {
            case ToolType::Select:
                m_selectAction->setChecked(true);
                break;
            case ToolType::CreatePoint:
                m_pointAction->setChecked(true);
                break;
            case ToolType::CreateLine:
                m_lineAction->setChecked(true);
                break;
            case ToolType::CreateCircle:
                m_circleAction->setChecked(true);
                break;
            case ToolType::CreateIntersection:
                m_intersectionAction->setChecked(true);
                break;
            case ToolType::CreateMidpoint:
                m_midpointAction->setChecked(true);
                break;
            case ToolType::CreateParallel:
                m_parallelAction->setChecked(true);
                break;
            case ToolType::CreatePerpendicular:
                m_perpendicularAction->setChecked(true);
                break;
            case ToolType::CreatePerpendicularFoot:
                m_perpendicularFootAction->setChecked(true);
                break;
        }
}