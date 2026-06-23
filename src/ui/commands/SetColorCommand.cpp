#include "SetColorCommand.h"
#include "ui/SceneAdapter.h"
#include "ui/GeoPointItem.h"
#include "ui/GeoLinearObjectItem.h"
#include "ui/GeoCircleItem.h"

SetColorCommand::SetColorCommand(SceneAdapter* adapter, GeoObject* object, const QColor& newColor)
    : m_adapter(adapter), m_object(object), m_newColor(newColor)
{
    // Alte Farbe aus dem Item lesen
    if (auto* item = m_adapter->itemFor(m_object)) {
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            m_oldColor = point_item->pen().color();
        }else if (auto* linear_item = dynamic_cast<GeoLinearObjectItem*>(item)) {
            m_oldColor = linear_item->pen().color();
        } else if (auto* circle_item = dynamic_cast<GeoCircleItem*>(item)) {
            m_oldColor = circle_item->pen().color();
        }
    }
}

void SetColorCommand::applyColor(SceneAdapter* adapter, GeoObject* obj, const QColor& color) {
    auto* item = adapter->itemFor(obj);
    if (item == nullptr) {
        return;
    }

    if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
        QPen pen = point_item->pen(); pen.setColor(color); point_item->setPen(pen);
    } else if (auto* linear_item = dynamic_cast<GeoLinearObjectItem*>(item)) {
        QPen pen = linear_item->pen(); pen.setColor(color); linear_item->setPen(pen);
    } else if (auto* circle_item = dynamic_cast<GeoCircleItem*>(item)) {
        QPen pen = circle_item->pen(); pen.setColor(color); circle_item->setPen(pen);
    }
}

void SetColorCommand::execute() { applyColor(m_adapter, m_object, m_newColor); }
void SetColorCommand::undo()    { applyColor(m_adapter, m_object, m_oldColor); }