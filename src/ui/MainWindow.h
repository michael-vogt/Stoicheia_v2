#pragma once

#include <QMainWindow>
#include <QAction>

#include "DrawingBoard.h"
#include "SceneAdapter.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    DrawingBoard* m_drawingBoard = nullptr;
    QAction* m_undoAction = nullptr;
    QAction* m_redoAction = nullptr;

    QAction* m_selectAction = nullptr;
    QAction* m_pointAction = nullptr;
    QAction* m_lineAction = nullptr;
    QAction* m_circleAction = nullptr;
    QAction* m_intersectionAction = nullptr;
    QAction* m_midpointAction = nullptr;
    QAction* m_parallelAction = nullptr;
    QAction* m_perpendicularAction = nullptr;
    QAction* m_perpendicularFootAction = nullptr;

    QToolBar* m_toolbar = nullptr;

    void setupToolBar();
    void setupMenu();
    void setupStatusBar() const;
    void updateUndoRedo() const;
    void toggleTools(const QAction* selectedAction) const;
    void checkTool(ToolType type);

public:
    explicit MainWindow(const QString& title = "Stoicheia", QWidget* parent = nullptr);

    DrawingBoard* drawingBoard() const { return m_drawingBoard; }
};
