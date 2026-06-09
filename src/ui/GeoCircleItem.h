#pragma once
#include "GeoGraphicsItem.h"
#include "../geometry/Circle.h"
#include <QPen>
#include <QBrush>


constexpr double DEFAULT_CIRCLE_PENWIDTH_NORMAL = 1.5;
constexpr double DEFAULT_CIRCLE_PENWIDTH_THICK = 2.5;

class GeoCircleItem : public GeoGraphicsItem {
public:
    explicit GeoCircleItem(Circle* circle, QGraphicsItem* parent = nullptr);

    [[nodiscard]] auto boundingRect() const -> QRectF override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    [[nodiscard]] auto circle() const -> Circle* { return m_circle; }

    void setPen(const QPen& pen) { m_pen = pen; update(); }
    void setBrush(const QBrush& brush) { m_brush = brush; update(); }

    auto contains(const QPointF &point) -> bool override;

protected:
    void updateGeometry() override;

private:
    Circle* m_circle;
    QPen    m_pen   = QPen(Qt::black, DEFAULT_CIRCLE_PENWIDTH_NORMAL);
    QBrush  m_brush = QBrush(Qt::NoBrush);
};
