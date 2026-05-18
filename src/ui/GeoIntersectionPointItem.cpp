#include "GeoIntersectionPointItem.h"
#include "dialogs/AppSettings.h"

#include <QPainter>

GeoIntersectionPointItem::GeoIntersectionPointItem(Point *point) : GeoPointItem(point) {
}

void GeoIntersectionPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!point()->isValid()) return;
    painter->setPen(m_pen);
    painter->setBrush(m_brush);
    const QRectF rect(-m_radius, -m_radius, 2*m_radius, 2*m_radius);
    painter->fillRect(rect, QBrush(AppSettings::instance().colors.construction));
}
