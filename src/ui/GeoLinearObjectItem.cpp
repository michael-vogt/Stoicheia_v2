#include "GeoLinearObjectItem.h"
#include "../geometry/Line.h"
#include "../geometry/Ray.h"
#include "../geometry/Segment.h"
#include <QPainter>
#include <cmath>

#include "LinearObjectType.h"

GeoLinearObjectItem::GeoLinearObjectItem(LinearObject* linearObject,
                                         QGraphicsItem* parent)
    : GeoGraphicsItem(linearObject, parent), m_linearObject(linearObject)
{
    m_linearObject->addDependent(this);
    GeoLinearObjectItem::updateGeometry();
}

QLineF GeoLinearObjectItem::computeVisibleLine() {
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
        m_type = LinearObjectType::Segment;
        return QLineF(x1, y1, x2, y2);

    }
    if (dynamic_cast<Ray*>(m_linearObject)) {
        // Halbgerade: von p1 in Richtung p2, weit genug
        m_type = LinearObjectType::Ray;
        return QLineF(x1, y1,
                      x1 + ux * m_extent,
                      y1 + uy * m_extent);

    }
    // Gerade (Line): in beide Richtungen
    m_type = LinearObjectType::Line;
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
    if (!m_linearObject->isValid()) return;
    if (m_highlighted)
        painter->setPen(QPen(QColor(255, 140, 0), 2.5));
    else if (m_selected)
        painter->setPen(QPen(Qt::blue, 2.5));
    else
        painter->setPen(m_pen);
    painter->drawLine(m_line);
}

void GeoLinearObjectItem::setPen(const QPen& pen) { m_pen = pen; update(); }
void GeoLinearObjectItem::setExtent(double extent) { m_extent = extent; update(); }

bool GeoLinearObjectItem::contains(const QPointF &point) {
    QPointF p1 = m_line.p1();
    QPointF p2 = m_line.p2();

    QPointF p1p2 = p2 - p1;
    QPointF p1p = point - p1;

    double ab2 = QPointF::dotProduct(p1p2, p1p2);
    if (ab2 == 0.0) return false;

    double t = QPointF::dotProduct(p1p, p1p2) / ab2;

    if (m_type == LinearObjectType::Segment) {
        if (t < 0.0 || t > 1.0) return false;
    } else if (m_type == LinearObjectType::Ray) {
        if (t < 0.0) return false;
    }

    QPointF proj = m_line.pointAt(t);
    double dist = QLineF(point, proj).length();
    return dist <= 8;

}
