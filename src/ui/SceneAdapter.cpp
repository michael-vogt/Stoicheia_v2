#include "SceneAdapter.h"
#include "../geometry/Point.h"
#include "../geometry/LinearObject.h"
#include "../geometry/Circle.h"
#include "GeoCircleItem.h"
#include "GeoPointItem.h"
#include "Structs.h"
#include "geometry/Point.h"
#include "ui/GeoLinearObjectItem.h"
#include <qnamespace.h>

SceneAdapter::SceneAdapter(Scene* geoScene, QGraphicsScene* qtScene)
    : m_geoScene(geoScene), m_qtScene(qtScene)
{}

auto SceneAdapter::addPoint(Point* point, const QPen& pen) -> GeoPointItem* {
    auto* item = new GeoPointItem(point);
    item->setPen(pen);
    m_qtScene->addItem(item);
    m_map[point] = item;
    applyStyle(point, item);
    return item;
}

auto SceneAdapter::addLinearObject(LinearObject* linearObject) -> GeoLinearObjectItem* {
    auto* item = new GeoLinearObjectItem(linearObject);
    m_qtScene->addItem(item);
    m_map[linearObject] = item;
    applyStyle(linearObject, item);
    return item;
}

auto SceneAdapter::addCircle(Circle* circle) -> GeoCircleItem* {
    auto* item = new GeoCircleItem(circle);
    m_qtScene->addItem(item);
    m_map[circle] = item;
    applyStyle(circle, item);
    return item;
}

auto SceneAdapter::addIntersectionPoint(IntersectionPoint *point, const QPen& pen) -> GeoIntersectionPointItem* {
    auto* item = new GeoIntersectionPointItem(point);
    item->setPen(pen);
    m_qtScene->addItem(item);
    m_map[point] = item;
    return item;
}

auto SceneAdapter::addIntersectionSet(IntersectionSet *intersectionSet) -> std::pair<GeoPointItem *, GeoPointItem *> {
    std::pair<GeoPointItem *, GeoPointItem *> result;
    if (intersectionSet == nullptr) {
        result.first = nullptr;
        result.second = nullptr;
    } else {
        m_intersectionSets.insert(intersectionSet);
        result.first = addIntersectionPoint(intersectionSet->first());
        result.second = addIntersectionPoint(intersectionSet->second());
    }
    return result;
}

void SceneAdapter::remove(GeoObject* geoObject) {
    if (geoObject == nullptr) {
        return;
    }
    if (auto* iset = dynamic_cast<IntersectionSet*>(geoObject)) {
        m_intersectionSets.erase(iset);
        // Die Kindpunkte sind member des IntersectionSet und in m_map eingetragen
        for (IntersectionPoint* point : {iset->first(), iset->second()}) {
            auto iter = m_map.find(point);
            if (iter != m_map.end()) {
                m_qtScene->removeItem(iter->second);
                delete iter->second;
                m_map.erase(iter);
            }
        }
    }
    auto iter = m_map.find(geoObject);
    if (iter != m_map.end()) {
        m_qtScene->removeItem(iter->second);
        delete iter->second;
        m_map.erase(iter);
    }
    // Geometrie-Objekt aus der Scene entfernen
    m_geoScene->softRemove(geoObject);
}

auto SceneAdapter::itemFor(GeoObject* geoObject) const -> GeoGraphicsItem* {
    auto iter = m_map.find(geoObject);
    return iter != m_map.end() ? iter->second : nullptr;
}

auto SceneAdapter::radiusPointFor(const Point *centerPoint) const -> Point * {
    for (const auto &geo: m_map | std::views::keys) {
        if (const auto* circle = dynamic_cast<Circle*>(geo)) {
            if (circle->center() == centerPoint) {
                return circle->radiusPoint();
            }
        }
    }
    return nullptr;
}

void SceneAdapter::select(GeoObject* geoObject) {
    if (m_map.contains(geoObject)) {
        m_map[geoObject]->setGeoSelected(true);
    }

    if (!m_selection.contains(geoObject)) {
        m_selection.insert(geoObject);
    }
    emit selectionChanged();
}

void SceneAdapter::deselect(GeoObject* geoObject) {
    if (m_map.contains(geoObject)) {
        m_map[geoObject]->setGeoSelected(false);
    }
    if (m_selection.contains(geoObject)) {
        m_selection.erase(geoObject);
    }
    emit selectionChanged();
}

void SceneAdapter::clearSelection() {
    for (const auto& geo : m_selection) {
        if (m_map.contains(geo)) {
            m_map[geo]->setGeoSelected(false);
        }
    }
    m_selection.clear();
    emit selectionChanged();
}

void SceneAdapter::clear() {
    for (const auto &item: m_map | std::views::values) {
        m_qtScene->removeItem(item);
        delete item;
    }
    m_map.clear();
    m_intersectionSets.clear();
}

void SceneAdapter::highlight(GeoObject *obj, bool isHighlighted) {
    auto iter = m_map.find(obj);
    if (iter == m_map.end()) {
        return;
    }
    iter->second->setHighlighted(isHighlighted);
}

void SceneAdapter::hide(GeoObject *obj) {
    m_styles[obj].visible = false;
    auto iter = m_map.find(obj);
    if (iter != m_map.end()) {
        iter->second->setVisible(false);
    }
}

void SceneAdapter::show(GeoObject *obj) {
    m_styles[obj].visible = true;
    auto iter = m_map.find(obj);
    if (iter != m_map.end()) {
        iter->second->setVisible(true);
    }
}

void SceneAdapter::setColor(GeoObject* obj, const QColor& color) {
    m_styles[obj].color = color;
    auto *item = itemFor(obj);
    if (item == nullptr) {
        return;
    }

    if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
        QPen pen = point_item->pen();
        pen.setColor(color);
        point_item->setPen(pen);
    } else if (auto* linear_item = dynamic_cast<GeoLinearObjectItem*>(item)) {
        QPen pen = linear_item->pen();
        pen.setColor(color);
        linear_item->setPen(pen);
    } else if (auto* circle_item = dynamic_cast<GeoCircleItem*>(item)) {
        QPen pen = circle_item->pen();
        pen.setColor(color);
        circle_item->setPen(pen);
    }
}

auto SceneAdapter::colorOf(GeoObject* obj) const -> QColor {
    auto iter = m_styles.find(obj);
    if (iter != m_styles.end() && iter->second.color.isValid()) {
        return iter->second.color;
    }

    // Fallback: Farbe ausa aktuellem Item lesen
    if (auto* item = itemFor(obj)) {
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            return point_item->pen().color();
        }
        if (auto* linear_item = dynamic_cast<GeoLinearObjectItem*>(item)) {
            return linear_item->pen().color();
        }
        if (auto* circle_item = dynamic_cast<GeoCircleItem*>(item)) {
            return circle_item->pen().color();
        }
    }

    return Qt::black;
}

auto SceneAdapter::visibleOf(GeoObject* obj) const -> bool {
    auto iter = m_styles.find(obj);
    if (iter != m_styles.end()) {
        return iter->second.visible;
    }
    if (auto* item = itemFor(obj)) {
        return item->isVisible();
    }
    return true;
}

void SceneAdapter::applyStyle(GeoObject* obj, GeoGraphicsItem* item) const {
    auto iter = m_styles.find(obj);
    if (iter == m_styles.end()) {
        return;
    }

    const ObjectStyle& style = iter->second;
    if (style.color.isValid()) {
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            QPen pen = point_item->pen();
            pen.setColor(style.color);
            point_item->setPen(pen);
        } else if (auto* linear_item = dynamic_cast<GeoLinearObjectItem*>(item)) {
            QPen pen = linear_item->pen();
            pen.setColor(style.color);
            linear_item->setPen(pen);
        } else if (auto* circle_item = dynamic_cast<GeoCircleItem*>(item)) {
            QPen pen = circle_item->pen();
            pen.setColor(style.color);
            circle_item->setPen(pen);
        }
    }
    item->setVisible(style.visible);
}



void SceneAdapter::removeGraphicsOnly(GeoObject *obj) {
    auto iter = m_map.find(obj);
    if (iter != m_map.end()) {
        m_qtScene->removeItem(iter->second);
        delete iter->second;
        m_map.erase(iter);
    }
}

void SceneAdapter::copySelection() {
    m_clipboard = selection();
}

void SceneAdapter::transferStyle(GeoObjectPair pair) {
    auto iter = m_styles.find(pair.oldGeoObject);
    if (iter == m_styles.end()) { 
        return;           // kein gespeicherter Style → nichts zu tun
    }
    m_styles[pair.newGeoObject] = iter->second;              // Style auf neuen Pointer umhängen
    m_styles.erase(iter);                         // alten Eintrag entfernen
    // Sofort auf das neue Item anwenden, falls es bereits existiert
    if (auto* item = itemFor(pair.newGeoObject)) {
        applyStyle(pair.newGeoObject, item);
    }
}
