#pragma once
#include <QGraphicsScene>
#include <unordered_map>
#include "../geometry/Scene.h"
#include "GeoGraphicsItem.h"
#include "GeoPointItem.h"
#include "GeoLinearObjectItem.h"
#include "GeoCircleItem.h"
#include "constructions/IntersectionSet.h"

// Verwaltet die Zuordnung zwischen geometrischen Objekten (Scene)
// und grafischen Objekten (QGraphicsScene).
//
// Ist die zentrale Anlaufstelle um Objekte hinzuzufügen und zu entfernen –
// beide Welten bleiben dadurch synchron.

class SceneAdapter {
public:
    SceneAdapter(Scene* geoScene, QGraphicsScene* qtScene);

    // Geometrie-Objekt + grafische Repräsentation hinzufügen
    GeoPointItem*         addPoint(Point* point);
    GeoLinearObjectItem*  addLinearObject(LinearObject* linearObject);
    GeoCircleItem*        addCircle(Circle* circle);
    std::pair<GeoPointItem*, GeoPointItem*> addIntersectionSet(IntersectionSet* intersectionSet);

    // Geometrie-Objekt und zugehöriges grafisches Objekt entfernen
    void remove(GeoObject* geoObject);

    // Zugriff auf das grafische Objekt zu einem Geometrie-Objekt
    GeoGraphicsItem* itemFor(GeoObject* geoObject) const;
    std::unordered_map<GeoObject*, GeoGraphicsItem*> geoGraphicsItems() { return m_map; }

private:
    Scene*          m_geoScene;
    QGraphicsScene* m_qtScene;

    std::unordered_map<GeoObject*, GeoGraphicsItem*> m_map;
};
