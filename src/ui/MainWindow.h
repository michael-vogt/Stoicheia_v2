#pragma once

#include <QMainWindow>
#include <QAction>

#include "DrawingBoard.h"
#include "SceneAdapter.h"
#include "io/FileManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    DrawingBoard* m_drawingBoard = nullptr;
    FileManager* m_fileManager = nullptr;
    QAction* m_undoAction = nullptr;
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

    //QToolBar* m_toolbar = nullptr;

    void setupToolBar();
    void setupMenu();
    void setupStatusBar() const;
    void updateUndoRedo() const;
    void toggleTools(const QAction* selectedAction) const;
    //void checkTool(ToolType type);

private slots:
    void onToolChanged(ToolType type);
    void onShortcutModeChanged(ShortcutMode mode);

public:
    explicit MainWindow(const QString& title = "Stoicheia", QWidget* parent = nullptr);

    DrawingBoard* drawingBoard() const { return m_drawingBoard; }
    FileManager* fileManager() const { return m_fileManager; }
};
