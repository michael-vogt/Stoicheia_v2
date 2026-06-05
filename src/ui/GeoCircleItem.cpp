#include "GeoCircleItem.h"
#include "dialogs/AppSettings.h"
#include <QPainter>

GeoCircleItem::GeoCircleItem(Circle* circle, QGraphicsItem* parent)
    : GeoGraphicsItem(circle, parent), m_circle(circle)
{
    m_circle->addDependent(this);
    GeoCircleItem::updateGeometry();
}

QRectF GeoCircleItem::boundingRect() const {
    double r = m_circle->radius();
    double extra = m_pen.widthF() / 2.0 + 1.0;
    return QRectF(-r - extra, -r - extra,
                   (r + extra) * 2, (r + extra) * 2);
}

void GeoCircleItem::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem*,
                           QWidget*)
{
    if (!m_circle->isValid()) return;
    if (m_highlighted)
        painter->setPen(QPen(AppSettings::instance().colors.highlighted, 2.5));
    else if (m_selected)
        painter->setPen(QPen(AppSettings::instance().colors.selected, 2.5));
    else
        painter->setPen(m_pen);

    const double r = m_circle->radius();
    painter->drawEllipse(QPointF(0, 0), r, r);
}

bool GeoCircleItem::contains(const QPointF &point) {
    if (!m_circle) return false;

    const QPointF p(m_circle->center()->x(), m_circle->center()->y());
    const double r = m_circle->radius();

    const double dx = point.x() - p.x();
    const double dy = point.y() - p.y();

    const double delta = std::abs(std::sqrt(dx * dx + dy * dy) - r);
    return delta <= 8;
}

void GeoCircleItem::updateGeometry() {
    setVisible(isValid());
    // Kreis zentriert auf Mittelpunkt – Position in Szene setzen
    setPos(m_circle->center()->x(), m_circle->center()->y());
}
