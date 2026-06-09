#pragma once

#include <QGraphicsView>

#include "HitTest.h"
#include "SceneAdapter.h"
#include "Structs.h"
#include "ToolContext.h"
#include "commands/CommandStack.h"
#include "geometry/Scene.h"
#include "tools/Tool.h"
#include "Grid.h"
#include "Enums.h"
#include "InputManager.h"


constexpr double DEFAULT_DRAWINGBOARD_HITTEST_TOLERANCE = 8;
constexpr double DEFAULT_DRAWINGBOARD_GRID_SPACING = 50;

class DrawingBoard : public QGraphicsView {

    Q_OBJECT

public:
    explicit DrawingBoard(QWidget* parent = nullptr);

    // Tool setzen - übernimmt Ownership
    template<typename T, typename... Args>
    void setTool(ToolType type, Args&&... args) {
        if (m_activeTool) { 
            m_activeTool->deactivate();
        }
        m_activeTool = std::make_unique<T>(makeContext(), std::forward<Args>(args)...);
        m_activeToolType = type;
        m_inputManager->setActiveTool(m_activeTool.get());
        m_activeTool->activate();
        emit toolChanged(type);
    }

    [[nodiscard]] auto activeTool() const -> Tool* { return m_activeTool.get(); }
    [[nodiscard]] auto activeToolType() const -> ToolType { return m_activeToolType; }
    [[nodiscard]] auto inputManager() const -> InputManager* { return m_inputManager; }
    void showStatusLeft(const QString& text) { emit statusBarTextChanged(StatusBarPart::Left, text); }
    void showStatusRight(const QString& text) { emit statusBarTextChanged(StatusBarPart::Right, text); }
    void updateToolType(ToolType type);

    // Raster
    void setGridVisible(bool visible);
    void setGridSpacing(double spacing);
    void resetView();
    auto grid() -> Grid* { return &m_grid; }

    // Geometrie und Adapter
    auto geoScene() -> Scene* { return &m_geoScene; }
    auto qtScene() -> QGraphicsScene* { return &m_qtScene; }
    auto adapter() -> SceneAdapter* { return &m_adapter; }
    auto commandStack() -> CommandStack* { return &m_commandStack; }

    // Copy and paste
    void copySelection();
    void pasteSelection();

public slots: // NOLINT
    void applySettings();
    void onToolChangeRequested(ToolTypePair toolTypePair);
    void statusMessage(int sbp, const QString& text);

signals:
    //void statusMessageChanged(const QString& text, int timeout = 0);
    void escapePressed();
    void statusBarTextChanged(StatusBarPart sbp, const QString& text);
    void toolChanged(ToolType type);
    void shortcutModeChanged(ShortcutMode mode);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawWatermark(QPainter *painter) const;
    auto makeContext() -> ToolContext { return ToolContext{ .drawingBoard=this, .adapter=&m_adapter, .commandStack=&m_commandStack, .snapHelper=&m_snapHelper, .hitTest=&m_hitTest }; }

    // Geometrie (Reihenfolge wichtig für Initialisierung!)
    Scene m_geoScene;
    QGraphicsScene m_qtScene;
    SceneAdapter m_adapter;
    CommandStack m_commandStack;
    HitTest m_hitTest{&m_qtScene, DEFAULT_DRAWINGBOARD_HITTEST_TOLERANCE};
    InputManager* m_inputManager = nullptr;

    // Raster
    Grid m_grid;
    bool m_gridVisible = true;
    double m_gridSpacing = DEFAULT_DRAWINGBOARD_GRID_SPACING;

    // Zentrierung
    int m_resizeCount = 0;

    // Aktives Tool
    std::unique_ptr<Tool> m_activeTool;
    ToolType m_activeToolType;

    SnapHelper m_snapHelper{&m_qtScene, &m_grid};
};

