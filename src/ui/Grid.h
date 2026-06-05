#pragma once

#include <functional>
#include <QPainter>
#include <QRectF>
#include <QPointF>

class Grid {
public:
    // Zeichnen
    void drawBackground(QPainter* painter, const QRectF& rect) const;
    void drawLabels(QPainter* painter, std::function<QPointF(QPointF)> sceneToViewport, int viewWidth, int viewHeight) const;

    // Snapping
    QPointF snap(const QPointF& scenePos, bool forceSnap) const;
    QPointF snapToGrid(const QPointF& scenePos) const;

    // Einstellungen
    void setVisible(bool visible) { m_visible = visible; }
    void setSpacing(double spacing) { m_spacing = spacing; }
    void setSnapEnabled(bool enabled) { m_snapEnabled = enabled; }

    bool isVisible() const { return m_visible; }
    double spacing() const { return m_spacing; }
    bool isSnapEnabled() const { return m_snapEnabled; }

    void setAxisColor(const QColor& c) { m_axisColor = c; }
    void setGridColor(const QColor& c) { m_gridColor = c; }
    void setLabelColor(const QColor& c) { m_labelColor = c; }

private:
    bool m_visible = true;
    double m_spacing = 50.0;
    bool m_snapEnabled = false;

    QColor m_axisColor = QColor(160, 160, 160);
    QColor m_gridColor = QColor(220, 220, 220);
    QColor m_labelColor = QColor(120, 120, 120);
};