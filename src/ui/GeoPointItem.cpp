#include "GeoPointItem.h"
#include "Constants.h"
#include "dialogs/AppSettings.h"
#include <QPainter>

GeoPointItem::GeoPointItem(Point* point, QGraphicsItem* parent)
    : GeoGraphicsItem(point, parent), m_point(point)
{
    m_point->addDependent(this);
    setPos(m_point->x(), m_point->y());
}

auto GeoPointItem::boundingRect() const -> QRectF {
    return {-m_radius - 1, -m_radius - 1, (m_radius * 2) + 2, (m_radius * 2) + 2};
}

void GeoPointItem::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* /*option*/,
                          QWidget* /*widget*/)
{
    if (!m_point->isValid()) {
        return;
    }
    if (m_highlighted) {
        painter->setPen(QPen(AppSettings::instance().colors.highlighted, Constants::DrawingConstants::PENWIDTH_THICK));
    } else if (m_selected) {
        painter->setPen(QPen(AppSettings::instance().colors.selected, Constants::DrawingConstants::PENWIDTH_THICK));
    } else {
        painter->setPen(m_pen);
    }
    painter->drawEllipse(QPointF(0, 0), m_radius, m_radius);
}

auto GeoPointItem::contains(const QPointF &point) -> bool {
    if (m_point == nullptr) {
        return false;
    }

    return boundingRect().contains(point);
}

void GeoPointItem::updateGeometry() {
    // Punkt-Position in Qt-Szene aktualisieren
    setVisible(m_point->isValid());
    setPos(m_point->x(), m_point->y());
}
