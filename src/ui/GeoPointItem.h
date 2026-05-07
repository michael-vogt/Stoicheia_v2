#pragma once
#include "GeoGraphicsItem.h"
#include "../geometry/Point.h"
#include <QPen>
#include <QBrush>

class GeoPointItem : public GeoGraphicsItem {
public:
    explicit GeoPointItem(Point* point, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    void setRadius(double r);
    void setPen(const QPen& pen);
    void setBrush(const QBrush& brush);

protected:
    void updateGeometry() override;

private:
    Point*  m_point;
    double  m_radius  = 4.0;
    QPen    m_pen     = QPen(Qt::black, 1.5);
    QBrush  m_brush   = QBrush(Qt::white);
};
