#include "Grid.h"
#include "Structs.h"
#include <cmath>
#include <QFont>
#include <QPen>


constexpr double DEFAULT_GRID_RASTERSTART = 0.1;
constexpr double DEFAULT_GRID_EXTENT = 10000;
constexpr int DEFAULT_GRID_FONTSIZE = 8;

void Grid::drawBackground(QPainter* painter, const QRectF& rect) const {
    if (!m_visible) {
        return;
    }

    // Achsen
    painter->setPen(QPen(m_axisColor, DEFAULT_GRID_PENWIDTH_NORMAL));
    painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0));
    painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom()));

    // Rasterlinien
    painter->setPen(QPen(m_gridColor, DEFAULT_GRID_PENWIDTH_THIN));
    double left = std::floor(rect.left() / m_spacing) * m_spacing;
    double top = std::floor(rect.top() / m_spacing) * m_spacing;

    for (double pos_x = left; pos_x <= rect.right(); pos_x += m_spacing) {
        if (std::abs(pos_x) > DEFAULT_GRID_RASTERSTART) {
            painter->drawLine(QPointF(pos_x, rect.top()), QPointF(pos_x, rect.bottom()));
        }
    }

    for (double pos_y = top; pos_y <= rect.bottom(); pos_y += m_spacing) {
        if (std::abs(pos_y) > DEFAULT_GRID_RASTERSTART) {
            painter->drawLine(QPointF(rect.left(), pos_y), QPointF(rect.right(), pos_y));
        }
    }
}

void Grid::drawLabels(QPainter* painter, const std::function<QPointF(QPointF)>& toViewport, IntPair viewSize) const {
    if (!m_visible) {
        return;
    }

    QFont font = painter->font();
    font.setPointSize(DEFAULT_GRID_FONTSIZE);
    painter->setFont(font);
    painter->setPen(m_labelColor);

    const int margin   = 4;
    const int tickSize = 4;

    QPointF origin = toViewport(QPointF(0,0));
    int origin_x = static_cast<int>(origin.x());
    int origin_y = static_cast<int>(origin.y());

    // X-Achse
    int labelY = static_cast<int>(std::clamp(
        static_cast<double>(origin_y + margin + 12),
        static_cast<double>(margin + 12),
        static_cast<double>(viewSize.height - margin)));

    // sichtbaren Bereich in Szenenkoordinaten approximieren
    // (wird von DrawingBoard als rect übergeben)
    for (double pos_x = -DEFAULT_GRID_EXTENT; pos_x <= DEFAULT_GRID_EXTENT; pos_x += m_spacing) {
        if (std::abs(pos_x) < DEFAULT_GRID_RASTERSTART) { 
            continue;
        }
        QPointF viewportPoint = toViewport(QPointF(pos_x, 0));
        if (viewportPoint.x() < 0 || viewportPoint.x() > viewSize.width) {
            continue;
        }
        int viewport_x = static_cast<int>(viewportPoint.x());
        painter->drawLine(viewport_x, origin_y - tickSize, viewport_x, origin_y + tickSize);
        painter->drawText(QRect(viewport_x-20, labelY-12, 40, 14),
                          Qt::AlignHCenter,
                          QString::number(static_cast<int>(std::round(pos_x))));
    }

    // Y-Achse
    int labelX = static_cast<int>(std::clamp(
        static_cast<double>(origin_x + margin),
        static_cast<double>(margin),
        static_cast<double>(viewSize.width - 40 - margin)));

    for (double pos_y = -DEFAULT_GRID_EXTENT; pos_y <= DEFAULT_GRID_EXTENT; pos_y += m_spacing) {
        if (std::abs(pos_y) < DEFAULT_GRID_RASTERSTART) {
            continue;
        }
        QPointF viewportPoint = toViewport(QPointF(0, pos_y));
        if (viewportPoint.y() < 0 || viewportPoint.y() > viewSize.height) {
            continue;
        }
        int viewport_y = static_cast<int>(viewportPoint.y());
        painter->drawLine(origin_x - tickSize, viewport_y, origin_x + tickSize, viewport_y);
        painter->drawText(QRect(labelX, viewport_y-7, 38, 14),
                          Qt::AlignLeft | Qt::AlignVCenter,
                          QString::number(static_cast<int>(std::round(pos_y))));
    }

    painter->drawText(QRect(origin_x+margin, origin_y+margin, 20, 14), Qt::AlignLeft, "0");
}

QPointF Grid::snap(const QPointF& scenePos, const bool forceSnap) const {
    if (m_snapEnabled || forceSnap) {
        return snapToGrid(scenePos);
    }
    return scenePos;
}

auto Grid::snapToGrid(const QPointF& scenePos) const -> QPointF {
    double pos_x = std::round(scenePos.x() / m_spacing) * m_spacing;
    double pos_y = std::round(scenePos.y() / m_spacing) * m_spacing;
    return {pos_x, pos_y};
}