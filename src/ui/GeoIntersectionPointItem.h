#pragma once
#include "GeoPointItem.h"

class GeoIntersectionPointItem : public GeoPointItem {
public:
    explicit GeoIntersectionPointItem(Point* point);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
