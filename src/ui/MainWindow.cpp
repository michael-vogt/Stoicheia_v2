#include "MainWindow.h"
#include "ui_mainwindow.h"

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
#include <QLayout>
#include <QTranslator>

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

MainWindow::MainWindow(const QString& title, QTranslator* translator, QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_translator(translator) {
    ui->setupUi(this);
    setWindowTitle(title);

    m_drawingBoard = new DrawingBoard(this);
    //ui->drawingBoardContainer->layout()->addWidget(m_drawingBoard);
    auto layout = new QVBoxLayout(ui->drawingBoardContainer);
    layout->setContentsMargins(0,0,0,0);;
    layout->addWidget(m_drawingBoard);

    m_statusLeft = new QLabel(this);
    m_statusRight = new QLabel(this);
    ui->statusbar->addPermanentWidget(m_statusLeft, 4);
    ui->statusbar->addPermanentWidget(m_statusRight, 1);

    m_fileManager = new FileManager(m_drawingBoard->geoScene(), m_drawingBoard->adapter(), m_drawingBoard->commandStack(), this);

    setupConnections();

    ui->toolBarGeometry->setEnabled(true);
    ui->toolBarConstructions->setEnabled(false);
    updateRecentFilesMenu();

    /*setupToolBar();
    setupMenu();
    setupStatusBar();*/

    connect(m_drawingBoard, &DrawingBoard::toolChanged, this, &MainWindow::onToolChanged);
    connect(m_drawingBoard, &DrawingBoard::shortcutModeChanged, this, &MainWindow::onShortcutModeChanged);
    connect(m_drawingBoard->commandStack(), &CommandStack::changed, this, &MainWindow::updateUndoRedo);
    connect(m_drawingBoard->commandStack(), &CommandStack::changed, m_fileManager, &FileManager::markChanged);
    connect(m_drawingBoard, &DrawingBoard::statusBarTextChanged, this, &MainWindow::setStatus);
    connect(m_drawingBoard, &DrawingBoard::escapePressed, [this]() {
        m_drawingBoard->setTool<SelectTool>(ToolType::Select);
    });

    qApp->installEventFilter(this);

    // Standard-Tool: Auswählen
    m_drawingBoard->setTool<SelectTool>(ToolType::Select);

    m_exportManager = new ExportManager(m_drawingBoard->scene(), this);
    m_exportManager->registerExporter(std::make_unique<PdfExporter>());
    m_exportManager->registerExporter(std::make_unique<PngExporter>());
    m_exportManager->registerExporter(std::make_unique<SvgExporter>());
}

void MainWindow::setupConnections() {
    // Toolbar Geometry
    connect(ui->actionSelect, &QAction::triggered, makeToolAction<SelectTool>(ToolType::Select));
    connect(ui->actionPoint, &QAction::triggered, makeToolAction<CreatePointTool>(ToolType::CreatePoint));
    connect(ui->actionLine, &QAction::triggered, makeToolAction<CreateLineTool>(ToolType::CreateLine, LinearObjectType::Line));
    connect(ui->actionRay, &QAction::triggered, makeToolAction<CreateLineTool>(ToolType::CreateRay, LinearObjectType::Ray));
    connect(ui->actionSegment, &QAction::triggered, makeToolAction<CreateLineTool>(ToolType::CreateSegment, LinearObjectType::Segment));
    connect(ui->actionCircle, &QAction::triggered, makeToolAction<CreateCircleTool>(ToolType::CreateCircle));

    // Toolbar Constructions
    connect(ui->actionIntersection, &QAction::triggered, makeToolAction<CreateIntersectionTool>(ToolType::CreateIntersection));
    connect(ui->actionMidpoint, &QAction::triggered, makeToolAction<CreateMidpointTool>(ToolType::CreateMidpoint));
    connect(ui->actionParallel, &QAction::triggered, makeToolAction<CreateParallelTool>(ToolType::CreateParallel));
    connect(ui->actionPerpendicular, &QAction::triggered, makeToolAction<CreatePerpendicularTool>(ToolType::CreatePerpendicular));
    connect(ui->actionPerpendicularFoot, &QAction::triggered, makeToolAction<CreatePerpendicularFootTool>(ToolType::CreatePerpendicularFoot));

    // Menu File
    ui->actionNew->setShortcut(QKeySequence::New);
    connect(ui->actionNew, &QAction::triggered, m_fileManager, &FileManager::newFile);

    ui->actionOpen->setShortcut(QKeySequence::Open);
    connect(ui->actionOpen, &QAction::triggered, [this]() {
        if (m_fileManager->open())
            updateRecentFilesMenu();
    });

    ui->actionSave->setShortcut(QKeySequence::Save);
    connect(ui->actionSave, &QAction::triggered, [this]() {
        if (m_fileManager->save())
            updateRecentFilesMenu();
    });

    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(ui->actionSaveAs, &QAction::triggered, [this]() {
        if (m_fileManager->saveAs())
            updateRecentFilesMenu();
    });

    connect(ui->actionExport, &QAction::triggered, [this]() {
       m_exportManager->exportWithDialog();
    });

    connect(ui->actionSettings, &QAction::triggered, [this]() {
        SettingsDialog dlg(AppSettings::instance(), this);
        connect(&dlg, &SettingsDialog::settingsChanged, m_drawingBoard, &DrawingBoard::applySettings);
        connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::switchLanguage);
        dlg.exec();
    });

    ui->actionQuit->setShortcut(QKeySequence::Close);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionUndo->setEnabled(false);
    connect(ui->actionUndo, &QAction::triggered, [this]() {
        m_drawingBoard->commandStack()->undo();
        updateUndoRedo();
    });

    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionRedo->setEnabled(false);
    connect(ui->actionRedo, &QAction::triggered, [this]() {
        m_drawingBoard->commandStack()->redo();
        updateUndoRedo();
    });
}

void MainWindow::onShortcutModeChanged(ShortcutMode mode) {
    switch (mode) {
        case ShortcutMode::None:
            ui->toolBarGeometry->setEnabled(true);
            ui->toolBarConstructions->setEnabled(true);
            break;
        case ShortcutMode::Geometry:
            ui->toolBarGeometry->setEnabled(true);
            ui->toolBarConstructions->setEnabled(false);
            break;
        case ShortcutMode::Construction:
            ui->toolBarGeometry->setEnabled(false);
            ui->toolBarConstructions->setEnabled(true);
            break;
    }
}

void MainWindow::onToolChanged(const ToolType type) const {
    auto check = [](QAction* a, const bool on) { if (a) a->setChecked(on); };

    check(ui->actionSelect,           type == ToolType::Select);
    check(ui->actionPoint,            type == ToolType::CreatePoint);
    check(ui->actionLine,             type == ToolType::CreateLine);
    check(ui->actionRay,              type == ToolType::CreateRay);
    check(ui->actionSegment,          type == ToolType::CreateSegment);
    check(ui->actionCircle,           type == ToolType::CreateCircle);
    check(ui->actionIntersection,     type == ToolType::CreateIntersection);
    check(ui->actionMidpoint,         type == ToolType::CreateMidpoint);
    check(ui->actionParallel,         type == ToolType::CreateParallel);
    check(ui->actionPerpendicular,    type == ToolType::CreatePerpendicular);
    check(ui->actionPerpendicularFoot,type == ToolType::CreatePerpendicularFoot);
}

void MainWindow::updateUndoRedo() const {
    ui->actionUndo->setEnabled(m_drawingBoard->commandStack()->canUndo());
    ui->actionRedo->setEnabled(m_drawingBoard->commandStack()->canRedo());

    QString undoDesc = m_drawingBoard->commandStack()->nextUndoDescription();
    QString redoDesc = m_drawingBoard->commandStack()->nextRedoDescription();

    ui->actionUndo->setText(undoDesc.isEmpty() ? tr("Rückgängig") : tr("Rückgängig: ") + undoDesc);
    ui->actionRedo->setText(redoDesc.isEmpty() ? tr("Wiederholen") : tr("Wiederholen: ") + redoDesc);
}

void MainWindow::updateRecentFilesMenu() {
    ui->menuRecent->clear();
    const auto& files = AppSettings::instance().general.recentFiles.files;

    if (files.isEmpty()) {
        auto* empty = ui->menuRecent->addAction(tr("(keine)"));
        empty->setEnabled(false);
        return;
    }

    for (const QString& file : files) {
        QString label = QFileInfo(file).absoluteFilePath();
        auto* action = ui->menuRecent->addAction(label);
        action->setToolTip(file);
        connect(action, &QAction::triggered, [this, file]() {
            if (m_fileManager->openFile(file))
                updateRecentFilesMenu();
        });
    }

    ui->menuRecent->addSeparator();
    const auto* clearAction = ui->menuRecent->addAction(tr("Liste leeren"));
    connect(clearAction, &QAction::triggered, [this]() {
        AppSettings::instance().general.recentFiles.files.clear();
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

void MainWindow::switchLanguage() {
    if (!m_translator)
        return;
    QString languageCode = AppSettings::instance().general.language;
    qApp->removeTranslator(m_translator);
    if (m_translator->load(QString(":/i18n/app_%1.qm").arg(languageCode)))
        qApp->installTranslator(m_translator);
    ui->retranslateUi(this);
}
