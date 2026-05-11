#include "GeoPointItem.h"
#include <QPainter>

GeoPointItem::GeoPointItem(Point* point, QGraphicsItem* parent)
    : GeoGraphicsItem(parent), m_point(point)
{
    m_point->addDependent(this);
    setPos(m_point->x(), m_point->y());
}

QRectF GeoPointItem::boundingRect() const {
    return QRectF(-m_radius - 1, -m_radius - 1,
                   m_radius * 2 + 2, m_radius * 2 + 2);
}

void GeoPointItem::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem*,
                          QWidget*)
{
    if (!m_point->isValid()) return;
    painter->setPen(m_pen);
    painter->setBrush(m_brush);
    painter->drawEllipse(QPointF(0, 0), m_radius, m_radius);
}

void GeoPointItem::updateGeometry() {
    // Punkt-Position in Qt-Szene aktualisieren
    setVisible(m_point->isValid());
    setPos(m_point->x(), m_point->y());
}

void GeoPointItem::setRadius(double r) { m_radius = r; update(); }
void GeoPointItem::setPen(const QPen& pen) { m_pen = pen; update(); }
void GeoPointItem::setBrush(const QBrush& brush) { m_brush = brush; update(); }
