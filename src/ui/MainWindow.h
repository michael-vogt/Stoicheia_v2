#pragma once

#include <QMainWindow>
#include <QAction>
#include <QLabel>

#include "DrawingBoard.h"
#include "io/FileManager.h"
#include "Enums.h"
#include "io/ExportManager.h"


namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString& title = "Stoicheia", QTranslator* translator = nullptr, QWidget* parent = nullptr);

    [[nodiscard]] auto drawingBoard() const -> DrawingBoard* { return m_drawingBoard; }
    [[nodiscard]] auto fileManager() const -> FileManager* { return m_fileManager; }

    auto eventFilter(QObject* object, QEvent* event) -> bool override;

public slots: // NOLINT
    void setStatus(StatusBarPart sbp, const QString& text) const;
    void switchLanguage();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupConnections();
    void updateUndoRedo() const;
    void updateRecentFilesMenu();

    template <typename ToolT>
    auto makeToolAction(ToolType type) const {
        return [this, type]() {
            m_drawingBoard->setTool<ToolT>(type);
        };
    }

    template <typename ToolT>
    auto makeToolAction(ToolType type, LinearObjectType loType) const {
        return [this, type, loType]() {
            m_drawingBoard->setTool<ToolT>(type, loType);
        };
    }

    Ui::MainWindow* ui;
    DrawingBoard* m_drawingBoard = nullptr;
    FileManager* m_fileManager = nullptr;
    QLabel* m_statusLeft = nullptr;
    QLabel* m_statusRight = nullptr;
    ExportManager* m_exportManager = nullptr;
    QTranslator* m_translator = nullptr;

private slots: // NOLINT
    void onShortcutModeChanged(ShortcutMode mode) const;
    void onToolChanged(ToolType type) const;
};
