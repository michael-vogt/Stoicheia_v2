#pragma once
#include "GeoGraphicsItem.h"
#include "../geometry/Circle.h"
#include <QPen>
#include <QBrush>

class GeoCircleItem : public GeoGraphicsItem {
public:
    explicit GeoCircleItem(Circle* circle, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    void setPen(const QPen& pen);
    void setBrush(const QBrush& brush);

    bool contains(const QPointF &point) override;

protected:
    void updateGeometry() override;

private:
    Circle* m_circle;
    QPen    m_pen   = QPen(Qt::black, 1.5);
    QBrush  m_brush = QBrush(Qt::NoBrush);
};
