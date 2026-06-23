#include "GeoCircleItem.h"
#include "Constants.h"
#include "dialogs/AppSettings.h"
#include <QPainter>


using namespace Constants;

GeoCircleItem::GeoCircleItem(Circle* circle, QGraphicsItem* parent)
    : GeoGraphicsItem(circle, parent), m_circle(circle)
{
    m_circle->addDependent(this);
    GeoCircleItem::updateGeometry();
}

auto GeoCircleItem::boundingRect() const -> QRectF {
    double radius = m_circle->radius();
    double extra = (m_pen.widthF() / 2) + 1.0;
    return {-radius - extra, -radius - extra,
                   (radius + extra) * 2, (radius + extra) * 2};
}

void GeoCircleItem::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* /*option*/,
                           QWidget* /*widget*/)
{
    if (!m_circle->isValid()) {
        return;
    }
    if (m_highlighted) {
        painter->setPen(QPen(AppSettings::instance().colors.highlighted, DrawingConstants::PENWIDTH_THICK));
    } else if (m_selected) {
        painter->setPen(QPen(AppSettings::instance().colors.selected, DrawingConstants::PENWIDTH_THICK));
    } else {
        painter->setPen(m_pen);
    }

    const double radius = m_circle->radius();
    painter->drawEllipse(QPointF(0, 0), radius, radius);
}

auto GeoCircleItem::contains(const QPointF &point) -> bool {
    if (m_circle == nullptr) { 
        return false;
    }

    const QPointF center(m_circle->center()->x(), m_circle->center()->y());
    const double radius = m_circle->radius();

    const double delta_x = point.x() - center.x();
    const double delta_y = point.y() - center.y();

    const double delta = std::abs(std::sqrt((delta_x * delta_x) + (delta_y * delta_y)) - radius);
    return delta <= UiMetricsConstants::HIT_TOLERANCE;
}

void GeoCircleItem::updateGeometry() {
    setVisible(isValid());
    // Kreis zentriert auf Mittelpunkt – Position in Szene setzen
    setPos(m_circle->center()->x(), m_circle->center()->y());
}
