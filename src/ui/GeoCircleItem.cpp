#include "GeoCircleItem.h"
#include <QPainter>

GeoCircleItem::GeoCircleItem(Circle* circle, QGraphicsItem* parent)
    : GeoGraphicsItem(parent), m_circle(circle)
{
    m_circle->addDependent(this);
    updateGeometry();
}

void GeoCircleItem::updateGeometry() {
    setVisible(isValid());
    // Kreis zentriert auf Mittelpunkt – Position in Szene setzen
    setPos(m_circle->center()->x(), m_circle->center()->y());
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
    double r = m_circle->radius();
    painter->setPen(m_pen);
    painter->setBrush(m_brush);
    painter->drawEllipse(QPointF(0, 0), r, r);
}

void GeoCircleItem::setPen(const QPen& pen) { m_pen = pen; update(); }
void GeoCircleItem::setBrush(const QBrush& brush) { m_brush = brush; update(); }
