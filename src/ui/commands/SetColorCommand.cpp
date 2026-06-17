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
        if (auto* pi = dynamic_cast<GeoPointItem*>(item))
            m_oldColor = pi->pen().color();
        else if (auto* li = dynamic_cast<GeoLinearObjectItem*>(item))
            m_oldColor = li->pen().color();
        else if (auto* ci = dynamic_cast<GeoCircleItem*>(item))
            m_oldColor = ci->pen().color();
    }
}

void SetColorCommand::applyColor(SceneAdapter* adapter, GeoObject* obj, const QColor& color) {
    auto* item = adapter->itemFor(obj);
    if (!item) return;

    if (auto* pi = dynamic_cast<GeoPointItem*>(item)) {
        QPen pen = pi->pen(); pen.setColor(color); pi->setPen(pen);
    } else if (auto* li = dynamic_cast<GeoLinearObjectItem*>(item)) {
        QPen pen = li->pen(); pen.setColor(color); li->setPen(pen);
    } else if (auto* ci = dynamic_cast<GeoCircleItem*>(item)) {
        QPen pen = ci->pen(); pen.setColor(color); ci->setPen(pen);
    }
}

void SetColorCommand::execute() { applyColor(m_adapter, m_object, m_newColor); }
void SetColorCommand::undo()    { applyColor(m_adapter, m_object, m_oldColor); }