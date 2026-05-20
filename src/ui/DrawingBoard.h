#pragma once

#include <QGraphicsView>

#include "HitTest.h"
#include "SceneAdapter.h"
#include "ToolContext.h"
#include "commands/CommandStack.h"
#include "geometry/Scene.h"
#include "tools/Tool.h"
#include "Grid.h"
#include "MainWindow.h"
#include "Enums.h"


class DrawingBoard : public QGraphicsView {
    Q_OBJECT
    // Geometrie (Reihenfolge wichtig für Initialisierung!)
    Scene m_geoScene;
    QGraphicsScene m_qtScene;
    SceneAdapter m_adapter;
    CommandStack m_commandStack;
    HitTest m_hitTest{&m_qtScene, 8.0};

    // Pan
    bool m_panning = false;
    bool m_spacePressed = false;
    QPoint m_panStart;

    // Snap
    bool m_snapping = false;

    // Raster
    Grid m_grid;
    bool m_gridVisible = true;
    double m_gridSpacing = 50.0;

    // Zentrierung
    int m_resizeCount = 0;

    // Aktives Tool
    std::unique_ptr<Tool> m_activeTool;
    ToolType m_activeToolType;

    SnapHelper m_snapHelper{&m_qtScene, &m_grid};

    ShortcutMode m_shortcutMode = ShortcutMode::None;
    void handleShortcutKey(QKeyEvent* event);
    void setShortcutMode(ShortcutMode mode);

    void drawWatermark(QPainter *painter) const;
    ToolContext makeContext() { return ToolContext{ this, &m_adapter, &m_commandStack, &m_snapHelper, &m_hitTest }; }

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
public:
    explicit DrawingBoard(QWidget* parent = nullptr);

    // Tool setzen - übernimmt Ownership
    template<typename T, typename... Args>
    void setTool(ToolType type, Args&&... args) {
        if (m_activeTool) m_activeTool->deactivate();
        m_activeTool = std::make_unique<T>(makeContext(), std::forward<Args>(args)...);
        m_activeToolType = type;
        m_shortcutMode = ShortcutMode::None;
        m_activeTool->activate();
        emit toolChanged(type);
    }

    Tool* activeTool() const { return m_activeTool.get(); }
    ToolType activeToolType() const { return m_activeToolType; }
    //void showStatus(const QString& message) { emit statusMessageChanged(message); }
    void showStatusLeft(const QString& text) { emit statusBarTextChanged(StatusBarPart::Left, text); }
    void showStatusRight(const QString& text) { emit statusBarTextChanged(StatusBarPart::Right, text); }
    void updateToolType(ToolType type);
    void setSnapping(bool snapping);

    // Raster
    void setGridVisible(bool visible);
    void setGridSpacing(double spacing);
    void resetView();
    Grid* grid() { return &m_grid; }

    // Geometrie und Adapter
    Scene* geoScene() { return &m_geoScene; }
    QGraphicsScene* qtScene() { return &m_qtScene; }
    SceneAdapter* adapter() { return &m_adapter; }
    CommandStack* commandStack() { return &m_commandStack; }

    signals:
    void escapePressed();
    //void statusMessageChanged(const QString& text, int timeout = 0);
    void statusBarTextChanged(StatusBarPart sbp, const QString& text);
    void toolChanged(ToolType type);
    void shortcutModeChanged(ShortcutMode mode);

    public slots:
    void applySettings();
};

