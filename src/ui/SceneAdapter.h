#pragma once
#include <QGraphicsScene>
#include <unordered_map>
#include "../geometry/Scene.h"
#include "GeoGraphicsItem.h"
#include "GeoPointItem.h"
#include "GeoLinearObjectItem.h"
#include "GeoCircleItem.h"
#include "GeoIntersectionPointItem.h"
#include "constructions/IntersectionSet.h"

// Verwaltet die Zuordnung zwischen geometrischen Objekten (Scene)
// und grafischen Objekten (QGraphicsScene).
//
// Ist die zentrale Anlaufstelle um Objekte hinzuzufügen und zu entfernen –
// beide Welten bleiben dadurch synchron.

class SceneAdapter : public QObject {
    Q_OBJECT
    Scene*          m_geoScene;
    QGraphicsScene* m_qtScene;

    std::unordered_map<GeoObject*, GeoGraphicsItem*> m_map;
    std::unordered_set<GeoObject*> m_selection;

public:
    SceneAdapter(Scene* geoScene, QGraphicsScene* qtScene);

    // Geometrie-Objekt + grafische Repräsentation hinzufügen
    GeoPointItem*         addPoint(Point* point, QPen pen = QPen(QColor(0,0,0), 1.5));
    GeoIntersectionPointItem* addIntersectionPoint(IntersectionPoint* point, QPen pen = QPen(QColor(64,64,64), 1.5));
    GeoLinearObjectItem*  addLinearObject(LinearObject* linearObject);
    GeoCircleItem*        addCircle(Circle* circle);
    std::pair<GeoPointItem*, GeoPointItem*> addIntersectionSet(IntersectionSet* intersectionSet);

    // Geometrie-Objekt und zugehöriges grafisches Objekt entfernen
    void remove(GeoObject* geoObject);

    // Zugriff auf das grafische Objekt zu einem Geometrie-Objekt
    GeoGraphicsItem* itemFor(GeoObject* geoObject) const;
    std::unordered_map<GeoObject*, GeoGraphicsItem*> geoGraphicsItems() { return m_map; }

    Point* radiusPointFor(const Point* centerPoint) const;

    Scene* geoScene() const { return m_geoScene; }

    void select(GeoObject* geoObject);
    void deselect(GeoObject* geoObject);
    void clearSelection();
    const std::unordered_set<GeoObject*>& selection() const { return m_selection; }

    signals:
    void selectionChanged();

};
