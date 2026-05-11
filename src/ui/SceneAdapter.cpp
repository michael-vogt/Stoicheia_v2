#include "SceneAdapter.h"
#include "../geometry/Point.h"
#include "../geometry/LinearObject.h"
#include "../geometry/Circle.h"

SceneAdapter::SceneAdapter(Scene* geoScene, QGraphicsScene* qtScene)
    : m_geoScene(geoScene), m_qtScene(qtScene)
{}

GeoPointItem* SceneAdapter::addPoint(Point* point, QPen pen) {
    auto* item = new GeoPointItem(point);
    item->setPen(pen);
    m_qtScene->addItem(item);
    m_map[point] = item;
    return item;
}

GeoIntersectionPointItem* SceneAdapter::addIntersectionPoint(IntersectionPoint *point, QPen pen) {
    auto* item = new GeoIntersectionPointItem(point);
    item->setPen(pen);
    m_qtScene->addItem(item);
    m_map[point] = item;
    return item;
}

std::pair<GeoPointItem *, GeoPointItem *> SceneAdapter::addIntersectionSet(IntersectionSet *intersectionSet) {
    std::pair<GeoPointItem *, GeoPointItem *> result;
    if (intersectionSet == nullptr) {
        result.first = nullptr;
        result.second = nullptr;
    } else {
        result.first = addIntersectionPoint(intersectionSet->first());
        result.second = addIntersectionPoint(intersectionSet->second());
    }
    return result;
}

GeoLinearObjectItem* SceneAdapter::addLinearObject(LinearObject* lo) {
    auto* item = new GeoLinearObjectItem(lo);
    m_qtScene->addItem(item);
    m_map[lo] = item;
    return item;
}

GeoCircleItem* SceneAdapter::addCircle(Circle* circle) {
    auto* item = new GeoCircleItem(circle);
    m_qtScene->addItem(item);
    m_map[circle] = item;
    return item;
}

void SceneAdapter::remove(GeoObject* geoObject) {
    auto it = m_map.find(geoObject);
    if (it != m_map.end()) {
        m_qtScene->removeItem(it->second);
        delete it->second;
        m_map.erase(it);
    }
    // Geometrie-Objekt aus der Scene entfernen
    m_geoScene->remove(geoObject);
}

GeoGraphicsItem* SceneAdapter::itemFor(GeoObject* geoObject) const {
    auto it = m_map.find(geoObject);
    return it != m_map.end() ? it->second : nullptr;
}

Point *SceneAdapter::radiusPointFor(const Point *centerPoint) const {
    for (const auto &geo: m_map | std::views::keys) {
        if (const auto* circle = dynamic_cast<Circle*>(geo))
            if (circle->center() == centerPoint) {
                return circle->radiusPoint();
            }
    }
    return nullptr;
}
