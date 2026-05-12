#pragma once
#include "GeoGraphicsItem.h"
#include "../geometry/LinearObject.h"
#include <QPen>

#include "LinearObjectType.h"

// Zeichnet Line, Ray oder Segment – der Typ bestimmt die Ausdehnung.
// - Segment: von p1 nach p2
// - Ray:     von p1 durch p2 bis zum Rand der Szene
// - Line:    durch p1 und p2 bis zu beiden Rändern der Szene

class GeoLinearObjectItem : public GeoGraphicsItem {
public:
    explicit GeoLinearObjectItem(LinearObject* linearObject,
                                  QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    void setPen(const QPen& pen);
    bool contains(const QPointF &point) override;

    // Ausdehnung für Line/Ray: wie weit über die definierten Punkte hinaus?
    void setExtent(double extent);

protected:
    void updateGeometry() override;

private:
    QLineF computeVisibleLine();

    LinearObject* m_linearObject;
    QPen          m_pen    = QPen(Qt::black, 1.5);
    double        m_extent = 10000.0; // weit genug für Line und Ray
    QLineF        m_line;             // aktuell zu zeichnende Linie
    LinearObjectType m_type;
};
