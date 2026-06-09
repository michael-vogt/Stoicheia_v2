#pragma once
#include "GeoGraphicsItem.h"
#include "../geometry/Point.h"
#include <QPen>
#include <QBrush>
#include "../Constants.h"

class GeoPointItem : public GeoGraphicsItem {
public:
    explicit GeoPointItem(Point* point, QGraphicsItem* parent = nullptr);

    [[nodiscard]] auto boundingRect() const -> QRectF override;
    [[nodiscard]] auto  point() const -> Point* { return m_point; };
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    auto contains(const QPointF& point) -> bool override;

    void setRadius(double radius) { m_radius = radius; update(); }
    void setPen(const QPen& pen) { m_pen = pen; update(); }
    void setBrush(const QBrush& brush) { m_brush = brush; update(); }

protected:
    void updateGeometry() override;

    QPen    m_pen     = QPen(Qt::black, Constants::DrawingConstants::PENWIDTH_NORMAL);
    QBrush  m_brush   = QBrush(Qt::white);
    double  m_radius  = Constants::DrawingConstants::POINT_RADIUS;

private:
    Point*  m_point = nullptr;
};
