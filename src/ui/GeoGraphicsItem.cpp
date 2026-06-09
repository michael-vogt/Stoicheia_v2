#include "GeoGraphicsItem.h"

GeoGraphicsItem::GeoGraphicsItem(GeoObject* object, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_object(object)
{
    // Damit Qt das Item korrekt neu zeichnet wenn sich die Position ändert
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void GeoGraphicsItem::recompute() {
    if (!m_valid) { 
        return;
    }
    prepareGeometryChange(); // Qt informieren bevor boundingRect sich ändert
    updateGeometry();
    update();                // Neuzeichnen anfordern
}

void GeoGraphicsItem::setHighlighted(bool highlighted) {
    if (m_highlighted == highlighted) {
        return;
    }
    m_highlighted = highlighted;
    update();
}
