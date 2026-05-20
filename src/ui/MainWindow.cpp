#include "MainWindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaEnum>
#include <QActionGroup>
#include <qfileinfo.h>
#include <QLabel>

#include "dialogs/SettingsDialog.h"
#include "io/PdfExporter.h"
#include "io/PngExporter.h"
#include "io/SvgExporter.h"
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
    connect(m_drawingBoard, &DrawingBoard::statusBarTextChanged, this, &MainWindow::setStatus);
    connect(m_drawingBoard, &DrawingBoard::escapePressed, [this]() {
        m_drawingBoard->setTool<SelectTool>(ToolType::Select);
        m_selectAction->setChecked(true);
    });

    qApp->installEventFilter(this);

    // Standard-Tool: Auswählen
    m_drawingBoard->setTool<SelectTool>(ToolType::Select);

    m_exportManager = new ExportManager(m_drawingBoard->scene(), this);
    m_exportManager->registerExporter(std::make_unique<PdfExporter>());
    m_exportManager->registerExporter(std::make_unique<PngExporter>());
    m_exportManager->registerExporter(std::make_unique<SvgExporter>());
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
    connect(openAction, &QAction::triggered, [this]() {
        if (m_fileManager->open())
            updateRecentFilesMenu();
    });

    m_recentMenu = fileMenu->addMenu(tr("Zuletzt geöffnet"));
    updateRecentFilesMenu();

    fileMenu->addSeparator();

    QAction* saveAction = fileMenu->addAction(tr("Speichern"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, [this]() {
        if (m_fileManager->save())
            updateRecentFilesMenu();
    });

    QAction* saveAsAction = fileMenu->addAction(tr("Speichern unter..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, [this]() {
        if (m_fileManager->saveAs())
            updateRecentFilesMenu();
    });

    QAction* exportAction = fileMenu->addAction(tr("Exportieren..."));
    connect(exportAction, &QAction::triggered, [this]() {
       m_exportManager->exportWithDialog();
    });

    /*QAction* svgAction = fileMenu->addAction(tr("Als SVG exportieren..."));
    connect(svgAction, &QAction::triggered, m_fileManager, &FileManager::exportSVG);*/

    fileMenu->addSeparator();

    QAction* settingsAction = fileMenu->addAction(tr("Einstellungen..."));
    connect(settingsAction, &QAction::triggered, [this]() {
        SettingsDialog dlg(AppSettings::instance(), this);
        connect(&dlg, &SettingsDialog::settingsChanged, m_drawingBoard, &DrawingBoard::applySettings);
        dlg.exec();
    });

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
}

void MainWindow::setupStatusBar() {
    m_statusLeft = new QLabel("", this);
    m_statusLeft->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    m_statusRight = new QLabel("", this);
    m_statusRight->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    statusBar()->addPermanentWidget(m_statusLeft, 4);
    statusBar()->addPermanentWidget(m_statusRight, 1);
}

void MainWindow::updateUndoRedo() const {
    m_undoAction->setEnabled(m_drawingBoard->commandStack()->canUndo());
    m_redoAction->setEnabled(m_drawingBoard->commandStack()->canRedo());

    QString undoDesc = m_drawingBoard->commandStack()->nextUndoDescription();
    QString redoDesc = m_drawingBoard->commandStack()->nextRedoDescription();

    m_undoAction->setText(undoDesc.isEmpty() ? tr("Rückgängig") : tr("Rückgängig: ") + undoDesc);
    m_redoAction->setText(redoDesc.isEmpty() ? tr("Wiederholen") : tr("Wiederholen: ") + redoDesc);
}

void MainWindow::updateRecentFilesMenu() {
    m_recentMenu->clear();
    const auto& files = AppSettings::instance().recent.files;

    if (files.isEmpty()) {
        auto* empty = m_recentMenu->addAction(tr("(keine)"));
        empty->setEnabled(false);
        return;
    }

    for (const QString& file : files) {
        QString label = QFileInfo(file).fileName();
        auto* action = m_recentMenu->addAction(label);
        action->setToolTip(file);
        connect(action, &QAction::triggered, [this, file]() {
            if (m_fileManager->openFile(file))
                updateRecentFilesMenu();
        });
    }

    m_recentMenu->addSeparator();
    auto* clearAction = m_recentMenu->addAction(tr("Liste leeren"));
    connect(clearAction, &QAction::triggered, [this]() {
        AppSettings::instance().recent.files.clear();
        AppSettings::instance().save();
        updateRecentFilesMenu();
    });

}

void MainWindow::setStatus(StatusBarPart sbp, const QString &text) const {
    if (sbp == StatusBarPart::Left && m_statusLeft) {
        m_statusLeft->setText(text);
    } else if (sbp == StatusBarPart::Right && m_statusRight) {
        m_statusRight->setText(text);
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Alt) {
            m_drawingBoard->setSnapping(true);
        }
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Alt) {
            m_drawingBoard->setSnapping(false);
        }
    }

    return QObject::eventFilter(object, event);
}
