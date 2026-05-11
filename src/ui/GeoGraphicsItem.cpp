#include "GeoGraphicsItem.h"

GeoGraphicsItem::GeoGraphicsItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    // Damit Qt das Item korrekt neu zeichnet wenn sich die Position ändert
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void GeoGraphicsItem::recompute() {
    if (!m_valid) return;
    prepareGeometryChange(); // Qt informieren bevor boundingRect sich ändert
    updateGeometry();
    update();                // Neuzeichnen anfordern
}
