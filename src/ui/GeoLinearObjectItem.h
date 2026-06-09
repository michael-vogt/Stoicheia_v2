#pragma once
#include "GeoGraphicsItem.h"
#include "../geometry/LinearObject.h"
#include <QPen>

#include "LinearObjectType.h"
#include "../Constants.h"

// Zeichnet Line, Ray oder Segment – der Typ bestimmt die Ausdehnung.
// - Segment: von p1 nach p2
// - Ray:     von p1 durch p2 bis zum Rand der Szene
// - Line:    durch p1 und p2 bis zu beiden Rändern der Szene


class GeoLinearObjectItem : public GeoGraphicsItem {
public:
    explicit GeoLinearObjectItem(LinearObject* linearObject,
                                  QGraphicsItem* parent = nullptr);

    [[nodiscard]] auto boundingRect() const -> QRectF override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    [[nodiscard]] auto linearObject() const -> LinearObject* { return m_linearObject; }

    void setPen(const QPen& pen) { m_pen = pen; update(); }
    void setExtent(double extent) { m_extent = extent; update(); }
    auto contains(const QPointF &point) -> bool override;

protected:
    void updateGeometry() override;

private:
    auto computeVisibleLine() -> QLineF;

    LinearObject* m_linearObject;
    QPen          m_pen    = QPen(Qt::black, Constants::DrawingConstants::PENWIDTH_NORMAL);
    double        m_extent = Constants::Geometry::LINE_EXTENT; // weit genug für Line und Ray
    QLineF        m_line;             // aktuell zu zeichnende Linie
    LinearObjectType m_type;
};
