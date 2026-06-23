#include "Grid.h"
#include "Constants.h"
#include "Structs.h"
#include <algorithm>
#include <cmath>
#include <QFont>
#include <QPen>
#include <sys/stat.h>


using namespace Constants;


void Grid::drawBackground(QPainter* painter, const QRectF& rect) const {
    if (!m_visible) {
        return;
    }

    // Achsen
    painter->setPen(QPen(m_axisColor, DrawingConstants::PENWIDTH_NORMAL));
    painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0));
    painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom()));

    // Rasterlinien
    painter->setPen(QPen(m_gridColor, DrawingConstants::PENWIDTH_THIN));
    double left = std::floor(rect.left() / m_spacing) * m_spacing;
    double top = std::floor(rect.top() / m_spacing) * m_spacing;

    for (double pos_x = left; pos_x <= rect.right(); pos_x += m_spacing) {
        if (std::abs(pos_x) > GridConstants::RASTER_START) {
            painter->drawLine(QPointF(pos_x, rect.top()), QPointF(pos_x, rect.bottom()));
        }
    }

    for (double pos_y = top; pos_y <= rect.bottom(); pos_y += m_spacing) {
        if (std::abs(pos_y) > GridConstants::RASTER_START) {
            painter->drawLine(QPointF(rect.left(), pos_y), QPointF(rect.right(), pos_y));
        }
    }
}

void Grid::drawLabels(QPainter* painter, const std::function<QPointF(QPointF)>& toViewport, IntPair viewSize, double zoomFactor) const {
    if (!m_visible) {
        return;
    }

    painter->save();

    QFont font = painter->font();
    font.setPointSize(Constants::DrawingConstants::FONT_SIZE);
    painter->setFont(font);

    const int margin   = 4;
    const int tickSize = static_cast<int>(std::clamp(4.0 * zoomFactor, 4.0, 12.0));
    const int penWidth = static_cast<int>(std::clamp(1.0 * zoomFactor, 1.0, 3.0));

    QPen tickPen(m_labelColor);
    tickPen.setWidth(penWidth);
    painter->setPen(tickPen);


    const double targetPixelSpacing = 80.0;
    double rawStep = targetPixelSpacing / (zoomFactor * m_spacing);
    double magnitude = std::pow(10.0, std::floor(std::log10(rawStep)));
    double normalized = rawStep / magnitude;
    double niceStep;
    if (normalized < 1.5) {
        niceStep = 1.0 * magnitude;
    } else if (normalized < 3.5) {
        niceStep = 2.0 * magnitude;
    } else if (normalized < 7.5) {
        niceStep = 5.0 * magnitude;
    } else {
        niceStep = 10.0 * magnitude;
    }
    double tickStep = niceStep * m_spacing;


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
    for (double pos_x = -GridConstants::EXTENT; pos_x <= GridConstants::EXTENT; pos_x += tickStep) {
        if (std::abs(pos_x) < GridConstants::RASTER_START) { 
            continue;
        }
        QPointF viewportPoint = toViewport(QPointF(pos_x, 0));
        if (viewportPoint.x() < 0 || viewportPoint.x() > viewSize.width) {
            continue;
        }
        int viewport_x = static_cast<int>(viewportPoint.x());
        painter->drawLine(viewport_x, origin_y - tickSize, viewport_x, origin_y + tickSize);
        QString label;
        double rounded = std::round(pos_x / tickStep) * tickStep;
        if (std::abs(rounded - std::round(rounded)) < Constants::NumericConstants::DOUBLE_EPS) {
            label = QString::number(static_cast<int>(std::round(rounded)));
        } else {
            label = QString::number(rounded, 'g', 4);
        }
        painter->drawText(QRect(viewport_x-20, labelY-12, 40, 14),
                          Qt::AlignHCenter, label);
    }

    // Y-Achse
    int labelX = static_cast<int>(std::clamp(
        static_cast<double>(origin_x + margin),
        static_cast<double>(margin),
        static_cast<double>(viewSize.width - 40 - margin)));

    for (double pos_y = -GridConstants::EXTENT; pos_y <= GridConstants::EXTENT; pos_y += tickStep) {
        if (std::abs(pos_y) < GridConstants::RASTER_START) {
            continue;
        }
        QPointF viewportPoint = toViewport(QPointF(0, pos_y));
        if (viewportPoint.y() < 0 || viewportPoint.y() > viewSize.height) {
            continue;
        }
        int viewport_y = static_cast<int>(viewportPoint.y());
        painter->drawLine(origin_x - tickSize, viewport_y, origin_x + tickSize, viewport_y);
        QString label;
        double rounded = std::round(pos_y / tickStep) * tickStep;
        if (std::round(rounded - std::round(rounded)) < Constants::NumericConstants::DOUBLE_EPS) {
            label = QString::number(static_cast<int>(std::round(rounded)));
        } else {
            label = QString::number(rounded, 'g', 4);
        }
        painter->drawText(QRect(labelX, viewport_y-7, 38, 14),
                          Qt::AlignLeft | Qt::AlignVCenter, label);
    }

    painter->drawText(QRect(origin_x+margin, origin_y+margin, 20, 14), Qt::AlignLeft, "0");

    painter->restore();
}

auto Grid::snap(const QPointF& scenePos, const bool forceSnap) const -> QPointF {
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