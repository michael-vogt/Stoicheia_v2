#include "GeoLinearObjectItem.h"
#include "GeoCircleItem.h"
#include "dialogs/AppSettings.h"
#include "../geometry/Ray.h"
#include "../geometry/Segment.h"
#include <QPainter>
#include <cmath>

#include "LinearObjectType.h"


constexpr double DEFAULT_LINEAROBJECT_SNAPWIDTH = 8;

GeoLinearObjectItem::GeoLinearObjectItem(LinearObject* linearObject,
                                         QGraphicsItem* parent)
    : GeoGraphicsItem(linearObject, parent), m_linearObject(linearObject)
{
    m_linearObject->addDependent(this);
    GeoLinearObjectItem::updateGeometry();
}

auto GeoLinearObjectItem::boundingRect() const -> QRectF {
    double extra = (m_pen.widthF() / 2) + 1.0;
    return QRectF(m_line.p1(), m_line.p2())
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void GeoLinearObjectItem::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* /*option*/,
                                 QWidget* /*widget*/)
{
    if (!m_linearObject->isValid()) {
        return;
    }
    if (m_highlighted) {
        painter->setPen(QPen(AppSettings::instance().colors.highlighted, DEFAULT_LINEAROBJECT_PENWIDTH_THICK));
    } else if (m_selected) {
        painter->setPen(QPen(AppSettings::instance().colors.selected, DEFAULT_CIRCLE_PENWIDTH_THICK));
    } else {
        painter->setPen(m_pen);
    }
    painter->drawLine(m_line);
}

auto GeoLinearObjectItem::contains(const QPointF &point) -> bool {
    QPointF point1 = m_line.p1();
    QPointF point2 = m_line.p2();

    QPointF p1p2 = point2 - point1;
    QPointF p1p = point - point1;

    double ab2 = QPointF::dotProduct(p1p2, p1p2);
    if (ab2 == 0.0) {
        return false;
    }

    double param_t = QPointF::dotProduct(p1p, p1p2) / ab2;

    if (m_type == LinearObjectType::Segment) {
        if (param_t < 0.0 || param_t > 1.0) {
            return false;
        }
    } else if (m_type == LinearObjectType::Ray) {
        if (param_t < 0.0) {
            return false;
        }
    }

    QPointF proj = m_line.pointAt(param_t);
    double dist = QLineF(point, proj).length();
    return dist <= DEFAULT_LINEAROBJECT_SNAPWIDTH;

}

void GeoLinearObjectItem::updateGeometry() {
    setVisible(isValid());
    m_line = computeVisibleLine();
}

auto GeoLinearObjectItem::computeVisibleLine() -> QLineF {
    double p1_x = m_linearObject->p1()->x();
    double p1_y = m_linearObject->p1()->y();
    double p2_x = m_linearObject->p2()->x();
    double p2_y = m_linearObject->p2()->y();

    double delta_x = p2_x - p1_x;
    double delta_y = p2_y - p1_y;
    double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));
    if (len < eps) {
        return {p1_x, p1_y, p2_x, p2_y};
    }

    double dir_x = delta_x / len;
    double dir_y = delta_y / len;

    if (dynamic_cast<Segment*>(m_linearObject) != nullptr) {
        // Strecke: exakt von p1 nach p2
        m_type = LinearObjectType::Segment;
        return {p1_x, p1_y, p2_x, p2_y};

    }
    if (dynamic_cast<Ray*>(m_linearObject) != nullptr) {
        // Halbgerade: von p1 in Richtung p2, weit genug
        m_type = LinearObjectType::Ray;
        return {p1_x, p1_y,
            p1_x + (dir_x * m_extent), (dir_y * m_extent) + p1_y};
    }
    // Gerade (Line): in beide Richtungen
    m_type = LinearObjectType::Line;
    return {p1_x - (dir_x * m_extent), p1_y - (dir_y * m_extent),
            p1_x + (dir_x * m_extent), (dir_y * m_extent) + p1_y};
}
