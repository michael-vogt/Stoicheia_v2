#pragma once

#include <QMainWindow>
#include <QAction>

#include "DrawingBoard.h"
#include "SceneAdapter.h"

class MainWindow : public QMainWindow {
    DrawingBoard* m_drawingBoard = nullptr;
    QAction* m_undoAction = nullptr;
    QAction* m_redoAction = nullptr;
    QToolBar* m_toolbar = nullptr;

    void setupToolBar();
    void setupMenu();
    void setupStatusBar() const;
    void updateUndoRedo() const;
    void toggleTools(const QAction* selectedAction) const;

public:
    explicit MainWindow(const QString& title = "Stoicheia", QWidget* parent = nullptr);

    DrawingBoard* drawingBoard() const { return m_drawingBoard; }
};
