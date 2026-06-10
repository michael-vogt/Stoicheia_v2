#pragma once

#include <functional>
#include <QPainter>
#include <QRectF>
#include <QPointF>
#include "Structs.h"
#include "../Constants.h"
#include "../GuiConstants.h"


using namespace Constants;
using namespace GuiConstants;

class Grid {
public:
    // Zeichnen
    void drawBackground(QPainter* painter, const QRectF& rect) const;
    void drawLabels(QPainter* painter, const std::function<QPointF(QPointF)>& sceneToViewport, IntPair viewSize) const;

    // Snapping
    [[nodiscard]] auto snap(const QPointF& scenePos, bool forceSnap) const -> QPointF;
    [[nodiscard]] auto snapToGrid(const QPointF& scenePos) const -> QPointF;

    // Einstellungen
    void setVisible(bool visible) { m_visible = visible; }
    void setSpacing(double spacing) { m_spacing = spacing; }
    void setSnapEnabled(bool enabled) { m_snapEnabled = enabled; }

    [[nodiscard]] auto isVisible() const -> bool { return m_visible; }
    [[nodiscard]] auto spacing() const -> double { return m_spacing; }
    [[nodiscard]] auto isSnapEnabled() const -> bool { return m_snapEnabled; }

    void setAxisColor(const QColor& color) { m_axisColor = color; }
    void setGridColor(const QColor& color) { m_gridColor = color; }
    void setLabelColor(const QColor& color) { m_labelColor = color; }

private:
    bool m_visible = true;
    double m_spacing = GridConstants::SPACING;
    bool m_snapEnabled = false;

    QColor m_axisColor = Colors::AXISCOLOR;
    QColor m_gridColor = Colors::GRIDCOLOR;
    QColor m_labelColor = Colors::LABELCOLOR;
};