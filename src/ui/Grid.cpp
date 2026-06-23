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

    double pos_x = left;
    while (pos_x <= rect.right()) {
        if (std::abs(pos_x) > GridConstants::RASTER_START) {
            painter->drawLine(QPointF(pos_x, rect.top()), QPointF(pos_x, rect.bottom()));
        }
        pos_x += m_spacing;
    }

    double pos_y = top;
    while (pos_y <= rect.bottom()) {
        if (std::abs(pos_y) > GridConstants::RASTER_START) {
            painter->drawLine(QPointF(rect.left(), pos_y), QPointF(rect.right(), pos_y));
        }
        pos_y += m_spacing;
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
    const int tick_size = static_cast<int>(std::clamp(4.0 * zoomFactor, 4.0, 12.0));
    const int pen_width = static_cast<int>(std::clamp(1.0 * zoomFactor, 1.0, 3.0));

    QPen tick_pen(m_labelColor);
    tick_pen.setWidth(pen_width);
    painter->setPen(tick_pen);


    const double target_pixel_spacing = 80.0;
    double raw_step = target_pixel_spacing / (zoomFactor * m_spacing);
    double magnitude = std::pow(10.0, std::floor(std::log10(raw_step)));
    double normalized = raw_step / magnitude;
    double nice_step;
    if (normalized < 1.5) {
        nice_step = 1.0 * magnitude;
    } else if (normalized < 3.5) {
        nice_step = 2.0 * magnitude;
    } else if (normalized < 7.5) {
        nice_step = 5.0 * magnitude;
    } else {
        nice_step = 10.0 * magnitude;
    }
    double tick_step = nice_step * m_spacing;


    QPointF origin = toViewport(QPointF(0,0));
    int origin_x = static_cast<int>(origin.x());
    int origin_y = static_cast<int>(origin.y());

    // X-Achse
    int label_y = static_cast<int>(std::clamp(
        static_cast<double>(origin_y + margin + 12),
        static_cast<double>(margin + 12),
        static_cast<double>(viewSize.height - margin)));

    // sichtbaren Bereich in Szenenkoordinaten approximieren
    // (wird von DrawingBoard als rect übergeben)
    double pos_x = -GridConstants::EXTENT;
    while (pos_x <= GridConstants::EXTENT) {
        if (std::abs(pos_x) < GridConstants::RASTER_START) { 
            pos_x += tick_step;
            //pos_x = (pos_x < 0) ? GridConstants::RASTER_START : pos_x + tick_step;
            continue;
        }
        QPointF viewport_point = toViewport(QPointF(pos_x, 0));
        if (viewport_point.x() < 0 || viewport_point.x() > viewSize.width) {
            pos_x += tick_step;
            continue;
        }
        int viewport_x = static_cast<int>(viewport_point.x());
        painter->drawLine(viewport_x, origin_y - tick_size, viewport_x, origin_y + tick_size);
        QString label;
        double rounded = std::round(pos_x / tick_step) * tick_step;
        if (std::abs(rounded - std::round(rounded)) < Constants::NumericConstants::DOUBLE_EPS) {
            label = QString::number(static_cast<int>(std::round(rounded)));
        } else {
            label = QString::number(rounded, 'g', 4);
        }
        painter->drawText(QRect(viewport_x-20, label_y-12, 40, 14),
                          Qt::AlignHCenter, label);
        pos_x += tick_step;
    }

    // Y-Achse
    int label_x = static_cast<int>(std::clamp(
        static_cast<double>(origin_x + margin),
        static_cast<double>(margin),
        static_cast<double>(viewSize.width - 40 - margin)));

    double pos_y = -GridConstants::EXTENT;
    while (pos_y <= GridConstants::EXTENT) {
        if (std::abs(pos_y) < GridConstants::RASTER_START) {
            pos_y += tick_step;
            continue;
        }
        QPointF viewport_point = toViewport(QPointF(0, pos_y));
        if (viewport_point.y() < 0 || viewport_point.y() > viewSize.height) {
            pos_y += tick_step;
            continue;
        }
        int viewport_y = static_cast<int>(viewport_point.y());
        painter->drawLine(origin_x - tick_size, viewport_y, origin_x + tick_size, viewport_y);
        QString label;
        double rounded = std::round(pos_y / tick_step) * tick_step;
        if (std::round(rounded - std::round(rounded)) < Constants::NumericConstants::DOUBLE_EPS) {
            label = QString::number(static_cast<int>(std::round(rounded)));
        } else {
            label = QString::number(rounded, 'g', 4);
        }
        painter->drawText(QRect(label_x, viewport_y-7, 38, 14),
                          Qt::AlignLeft | Qt::AlignVCenter, label);
        pos_y += tick_step;
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