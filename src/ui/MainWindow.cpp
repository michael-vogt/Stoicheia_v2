#include "MainWindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaEnum>
#include <QActionGroup>
#include <QLabel>

#include "tools/CreateCircleTool.h"
#include "tools/CreateIntersectionTool.h"
#include "tools/CreateLineTool.h"
#include "tools/CreateMidpointTool.h"
#include "tools/CreateParallelTool.h"
#include "tools/CreatePerpendicularFootTool.h"
#include "tools/CreatePerpendicularTool.h"
#include "tools/CreatePointTool.h"
#include "tools/SelectTool.h"

MainWindow::MainWindow(const QString& title, QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(title);

    m_drawingBoard = new DrawingBoard(this);
    setCentralWidget(m_drawingBoard);

    m_fileManager = new FileManager(m_drawingBoard->geoScene(), m_drawingBoard->adapter(), m_drawingBoard->commandStack(), this);

    setupToolBar();
    setupMenu();
    setupStatusBar();

    connect(m_drawingBoard, &DrawingBoard::toolChanged, this, &MainWindow::onToolChanged);
    connect(m_drawingBoard, &DrawingBoard::shortcutModeChanged, this, &MainWindow::onShortcutModeChanged);
    connect(m_drawingBoard->commandStack(), &CommandStack::changed, this, &MainWindow::updateUndoRedo);
    connect(m_drawingBoard->commandStack(), &CommandStack::changed, m_fileManager, &FileManager::markChanged);
    connect(m_drawingBoard, &DrawingBoard::statusMessageChanged, statusBar(), &QStatusBar::showMessage);
    connect(m_drawingBoard, &DrawingBoard::escapePressed, [this]() {
        m_drawingBoard->setTool<SelectTool>(ToolType::Select);
        m_selectAction->setChecked(true);
    });

    // Standard-Tool: Auswählen
    m_drawingBoard->setTool<SelectTool>(ToolType::Select);

}

void MainWindow::setupToolBar() {
    // ── Geometrie-Toolbar ────────────────────────────────────────────────────
    m_geoToolBar = addToolBar(tr("Geometrie"));
    m_geoToolBar->setMovable(false);

    auto* geoLabel = new QLabel(tr("Geometrie [G] >>"));
    geoLabel->setStyleSheet("font-weight: bold; color: gray;");
    m_geoToolBar->addWidget(geoLabel);
    m_geoToolBar->addSeparator();

    auto* geoGroup = new QActionGroup(this);
    geoGroup->setExclusive(true);

    auto addGeoAction = [&](const QString& label, auto slot) -> QAction* {
        auto* action = m_geoToolBar->addAction(label);
        action->setCheckable(true);
        geoGroup->addAction(action);
        connect(action, &QAction::triggered, this, slot);
        return action;
    };

    m_selectAction = addGeoAction(tr("Auswählen"), [this]() { m_drawingBoard->setTool<SelectTool>(ToolType::Select); });
    m_pointAction = addGeoAction(tr("Punkt [P]"), [this]() { m_drawingBoard->setTool<CreatePointTool>(ToolType::CreatePoint); });
    m_lineAction = addGeoAction(tr("Gerade [L]"), [this]() { m_drawingBoard->setTool<CreateLineTool>(ToolType::CreateLine, LinearObjectType::Line); });
    m_rayAction = addGeoAction(tr("Halbgerade [R]"), [this]() { m_drawingBoard->setTool<CreateLineTool>(ToolType::CreateRay, LinearObjectType::Ray); });
    m_segmentAction = addGeoAction(tr("Strecke [S]"), [this]() { m_drawingBoard->setTool<CreateLineTool>(ToolType::CreateSegment, LinearObjectType::Segment); });
    m_circleAction = addGeoAction(tr("Kreis [C]"), [this]() { m_drawingBoard->setTool<CreateCircleTool>(ToolType::CreateCircle); });
    m_selectAction->setChecked(true);

    // ── Konstruktions-Toolbar ────────────────────────────────────────────────────
    m_conToolBar = addToolBar(tr("Konstruktionen"));
    m_conToolBar->setMovable(false);

    auto* conLabel = new QLabel(tr("Konstruktionen [K] >>"));
    conLabel->setStyleSheet("font-weight: bold; color: gray;");
    m_conToolBar->addWidget(conLabel);
    m_conToolBar->addSeparator();

    auto* conGroup = new QActionGroup(this);
    conGroup->setExclusive(true);

    auto addConAction = [&](const QString& label, auto slot) -> QAction* {
        auto* action = m_conToolBar->addAction(label);
        action->setCheckable(true);
        conGroup->addAction(action);
        connect(action, &QAction::triggered, this, slot);
        return action;
    };

    m_intersectionAction = addConAction(tr("Schnittpunkt [S]"), [this]() { m_drawingBoard->setTool<CreateIntersectionTool>(ToolType::CreateIntersection); });
    m_midpointAction = addConAction(tr("Mittelpunkt [M]"), [this]() { m_drawingBoard->setTool<CreateMidpointTool>(ToolType::CreateMidpoint); });
    m_parallelAction = addConAction(tr("Parallele [P]"), [this]() { m_drawingBoard->setTool<CreateParallelTool>(ToolType::CreateParallel); });
    m_perpendicularAction = addConAction(tr("Senkrechte [E]"), [this]() { m_drawingBoard->setTool<CreatePerpendicularTool>(ToolType::CreatePerpendicular); });
    m_perpFootAction = addConAction(tr("Lotfußpunkt [L]"), [this]() { m_drawingBoard->setTool<CreatePerpendicularFootTool>(ToolType::CreatePerpendicularFoot); });

    // beim Start: Geometrie-Toolbar aktiv, Konstruktions-Toolbar deaktiviert
    m_geoToolBar->setEnabled(true);
    m_conToolBar->setEnabled(false);
}

void MainWindow::onShortcutModeChanged(ShortcutMode mode) {
    switch (mode) {
        case ShortcutMode::None:
            m_geoToolBar->setEnabled(true);
            m_conToolBar->setEnabled(true);
            break;
        case ShortcutMode::Geometry:
            m_geoToolBar->setEnabled(true);
            m_conToolBar->setEnabled(false);
            break;
        case ShortcutMode::Construction:
            m_geoToolBar->setEnabled(false);
            m_conToolBar->setEnabled(true);
            break;
    }
}

void MainWindow::onToolChanged(ToolType type) {
    auto check = [](QAction* a, bool on) { if (a) a->setChecked(on); };

    check(m_selectAction,        type == ToolType::Select);
    check(m_pointAction,         type == ToolType::CreatePoint);
    check(m_lineAction,          type == ToolType::CreateLine);
    check(m_rayAction,           type == ToolType::CreateRay);
    check(m_segmentAction,       type == ToolType::CreateSegment);
    check(m_circleAction,        type == ToolType::CreateCircle);
    check(m_intersectionAction,  type == ToolType::CreateIntersection);
    check(m_midpointAction,      type == ToolType::CreateMidpoint);
    check(m_parallelAction,      type == ToolType::CreateParallel);
    check(m_perpendicularAction, type == ToolType::CreatePerpendicular);
    check(m_perpFootAction,      type == ToolType::CreatePerpendicularFoot);

}

void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu(tr("Datei"));

    QAction* newAction = fileMenu->addAction(tr("Neu"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, m_fileManager, &FileManager::newFile);

    QAction* openAction = fileMenu->addAction(tr("Öffnen..."));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, m_fileManager, &FileManager::open);

    QAction* saveAction = fileMenu->addAction(tr("Speichern"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, m_fileManager, &FileManager::save);

    QAction* saveAsAction = fileMenu->addAction(tr("Speichern unter..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, m_fileManager, &FileManager::saveAs);

    fileMenu->addSeparator();

    QAction* svgAction = fileMenu->addAction(tr("Als SVG exportieren..."));
    connect(svgAction, &QAction::triggered, m_fileManager, &FileManager::exportSVG);

    fileMenu->addSeparator();

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

/*void MainWindow::checkTool(ToolType type) {
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
}*/