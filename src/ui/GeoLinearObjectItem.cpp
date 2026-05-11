#include "GeoLinearObjectItem.h"
#include "../geometry/Line.h"
#include "../geometry/Ray.h"
#include "../geometry/Segment.h"
#include <QPainter>
#include <cmath>

GeoLinearObjectItem::GeoLinearObjectItem(LinearObject* linearObject,
                                           QGraphicsItem* parent)
    : GeoGraphicsItem(parent), m_linearObject(linearObject)
{
    m_linearObject->addDependent(this);
    updateGeometry();
}

QLineF GeoLinearObjectItem::computeVisibleLine() const {
    double x1 = m_linearObject->p1()->x();
    double y1 = m_linearObject->p1()->y();
    double x2 = m_linearObject->p2()->x();
    double y2 = m_linearObject->p2()->y();

    double dx = x2 - x1;
    double dy = y2 - y1;
    double len = std::sqrt(dx*dx + dy*dy);
    if (len < 1e-10) return QLineF(x1, y1, x2, y2);

    double ux = dx / len, uy = dy / len;

    if (dynamic_cast<Segment*>(m_linearObject)) {
        // Strecke: exakt von p1 nach p2
        return QLineF(x1, y1, x2, y2);

    }
    if (dynamic_cast<Ray*>(m_linearObject)) {
        // Halbgerade: von p1 in Richtung p2, weit genug
        return QLineF(x1, y1,
                      x1 + ux * m_extent,
                      y1 + uy * m_extent);

    }
    // Gerade (Line): in beide Richtungen
    return QLineF(x1 - ux * m_extent, y1 - uy * m_extent,
                  x1 + ux * m_extent, y1 + uy * m_extent);
}

void GeoLinearObjectItem::updateGeometry() {
    setVisible(isValid());
    m_line = computeVisibleLine();
}

QRectF GeoLinearObjectItem::boundingRect() const {
    double extra = m_pen.widthF() / 2.0 + 1.0;
    return QRectF(m_line.p1(), m_line.p2())
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void GeoLinearObjectItem::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem*,
                                 QWidget*)
{
    painter->setPen(m_pen);
    painter->drawLine(m_line);
}

void GeoLinearObjectItem::setPen(const QPen& pen) { m_pen = pen; update(); }
void GeoLinearObjectItem::setExtent(double extent) { m_extent = extent; update(); }
