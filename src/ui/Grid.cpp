// ui/Grid.cpp
#include "Grid.h"
#include <cmath>
#include <QFont>
#include <QPen>

void Grid::drawBackground(QPainter* painter, const QRectF& rect) const {
    if (!m_visible) return;

    // Achsen
    painter->setPen(QPen(QColor(160,160,160), 1.5));
    painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0));
    painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom()));

    // Rasterlinien
    painter->setPen(QPen(QColor(220,220,220), 0.5));
    double left = std::floor(rect.left() / m_spacing) * m_spacing;
    double top = std::floor(rect.top() / m_spacing) * m_spacing;

    for (double x = left; x <= rect.right(); x += m_spacing)
        if (std::abs(x) > 0.1)
            painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));

    for (double y = top; y <= rect.bottom(); y += m_spacing)
        if (std::abs(y) > 0.1)
            painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
}

void Grid::drawLabels(QPainter* painter, std::function<QPointF(QPointF)> toViewport, int viewWidth, int viewHeight) const {
    if (!m_visible) return;

    QFont font = painter->font();
    font.setPointSize(8);
    painter->setFont(font);
    painter->setPen(QColor(120,120,120));

    const int margin   = 4;
    const int tickSize = 4;

    QPointF origin = toViewport(QPointF(0,0));
    int ox = static_cast<int>(origin.x());
    int oy = static_cast<int>(origin.y());

    // X-Achse
    int labelY = static_cast<int>(std::clamp(
        static_cast<double>(oy + margin + 12),
        static_cast<double>(margin + 12),
        static_cast<double>(viewHeight - margin)));

    // sichtbaren Bereich in Szenenkoordinaten approximieren
    // (wird von DrawingBoard als rect übergeben)
    for (double x = -10000; x <= 10000; x += m_spacing) {
        if (std::abs(x) < 0.1) continue;
        QPointF vp = toViewport(QPointF(x, 0));
        if (vp.x() < 0 || vp.x() > viewWidth) continue;
        int px = static_cast<int>(vp.x());
        painter->drawLine(px, oy - tickSize, px, oy + tickSize);
        painter->drawText(QRect(px-20, labelY-12, 40, 14),
                          Qt::AlignHCenter,
                          QString::number(static_cast<int>(std::round(x))));
    }

    // Y-Achse
    int labelX = static_cast<int>(std::clamp(
        static_cast<double>(ox + margin),
        static_cast<double>(margin),
        static_cast<double>(viewWidth - 40 - margin)));

    for (double y = -10000; y <= 10000; y += m_spacing) {
        if (std::abs(y) < 0.1) continue;
        QPointF vp = toViewport(QPointF(0, y));
        if (vp.y() < 0 || vp.y() > viewHeight) continue;
        int py = static_cast<int>(vp.y());
        painter->drawLine(ox - tickSize, py, ox + tickSize, py);
        painter->drawText(QRect(labelX, py-7, 38, 14),
                          Qt::AlignLeft | Qt::AlignVCenter,
                          QString::number(static_cast<int>(std::round(y))));
    }

    painter->drawText(QRect(ox+margin, oy+margin, 20, 14), Qt::AlignLeft, "0");
}

QPointF Grid::snapToGrid(const QPointF& scenePos) const {
    double x = std::round(scenePos.x() / m_spacing) * m_spacing;
    double y = std::round(scenePos.y() / m_spacing) * m_spacing;
    return QPointF(x, y);
}

QPointF Grid::snap(const QPointF& scenePos, bool forceSnap) const {
    if (m_snapEnabled || forceSnap)
        return snapToGrid(scenePos);
    return scenePos;
}