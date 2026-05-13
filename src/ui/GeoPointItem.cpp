#include "GeoPointItem.h"
#include <QPainter>

GeoPointItem::GeoPointItem(Point* point, QGraphicsItem* parent)
    : GeoGraphicsItem(point, parent), m_point(point)
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
    if (m_highlighted)
        painter->setPen(QPen(QColor(255, 140, 0), 2.5));
    else if (m_selected)
        painter->setPen(QPen(Qt::blue, 2.5));
    else
        painter->setPen(m_pen);
    //painter->setBrush(m_selected ? QBrush(Qt::cyan) : m_brush);
    painter->drawEllipse(QPointF(0, 0), m_radius, m_radius);
}

bool GeoPointItem::contains(const QPointF &point) {
    if (!m_point) return false;

    return boundingRect().contains(point);
}

void GeoPointItem::updateGeometry() {
    // Punkt-Position in Qt-Szene aktualisieren
    setVisible(m_point->isValid());
    setPos(m_point->x(), m_point->y());
}

void GeoPointItem::setRadius(double r) { m_radius = r; update(); }
void GeoPointItem::setPen(const QPen& pen) { m_pen = pen; update(); }
void GeoPointItem::setBrush(const QBrush& brush) { m_brush = brush; update(); }
