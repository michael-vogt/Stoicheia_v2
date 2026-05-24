#pragma once

#include <QMainWindow>
#include <QAction>
#include <QLabel>

#include "DrawingBoard.h"
#include "io/FileManager.h"
#include "Enums.h"
#include "io/ExportManager.h"

enum class ToolType;
enum class ShortcutMode;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow* ui;

    DrawingBoard* m_drawingBoard = nullptr;
    FileManager* m_fileManager = nullptr;

    /*QAction* m_undoAction = nullptr;
    QAction* m_redoAction = nullptr;
    
    QToolBar* m_geoToolBar = nullptr;
    QAction* m_selectAction = nullptr;
    QAction* m_pointAction = nullptr;
    QAction* m_lineAction = nullptr;
    QAction* m_rayAction = nullptr;
    QAction* m_segmentAction = nullptr;
    QAction* m_circleAction = nullptr;

    QToolBar* m_conToolBar = nullptr;
    QAction* m_intersectionAction = nullptr;
    QAction* m_midpointAction = nullptr;
    QAction* m_parallelAction = nullptr;
    QAction* m_perpendicularAction = nullptr;
    QAction* m_perpFootAction = nullptr;

    QMenu* m_recentMenu = nullptr;*/

    QLabel* m_statusLeft = nullptr;
    QLabel* m_statusRight = nullptr;

    ExportManager* m_exportManager = nullptr;

    QTranslator* m_translator = nullptr;

    //void setupToolBar();
    //void setupMenu();
    //void setupStatusBar();
    void setupConnections();
    void updateUndoRedo() const;
    void updateRecentFilesMenu();

    template <typename ToolT>
    auto makeToolAction(ToolType type) {
        return [this, type]() {
            m_drawingBoard->setTool<ToolT>(type);
        };
    }

private slots:
    void onToolChanged(ToolType type) const;
    void onShortcutModeChanged(ShortcutMode mode);

public:
    explicit MainWindow(const QString& title = "Stoicheia", QTranslator* translator = nullptr, QWidget* parent = nullptr);

    DrawingBoard* drawingBoard() const { return m_drawingBoard; }
    FileManager* fileManager() const { return m_fileManager; }

    bool eventFilter(QObject* object, QEvent* event);

public slots:
    void setStatus(StatusBarPart sbp, const QString& text) const;
    void switchLanguage();
};
