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
#include "dialogs/AppSettings.h"
#include "../Constants.h"

// Verwaltet die Zuordnung zwischen geometrischen Objekten (Scene)
// und grafischen Objekten (QGraphicsScene).
//
// Ist die zentrale Anlaufstelle um Objekte hinzuzufügen und zu entfernen –
// beide Welten bleiben dadurch synchron.


class SceneAdapter : public QObject {

    Q_OBJECT

public:
    SceneAdapter(Scene* geoScene, QGraphicsScene* qtScene);

    // Geometrie-Objekt + grafische Repräsentation hinzufügen
    auto addPoint(Point* point, const QPen& pen = QPen(AppSettings::instance().colors.point, Constants::DrawingConstants::PENWIDTH_NORMAL)) -> GeoPointItem*;
    auto addLinearObject(LinearObject* linearObject) -> GeoLinearObjectItem*;
    auto addCircle(Circle* circle) -> GeoCircleItem*;
    auto addIntersectionPoint(IntersectionPoint* point, const QPen& pen = QPen(AppSettings::instance().colors.construction, Constants::DrawingConstants::PENWIDTH_NORMAL)) -> GeoIntersectionPointItem*;
    auto addIntersectionSet(IntersectionSet* intersectionSet) -> std::pair<GeoPointItem*, GeoPointItem*>;

    // Geometrie-Objekt und zugehöriges grafisches Objekt entfernen
    void remove(GeoObject* geoObject);

    // Zugriff auf das grafische Objekt zu einem Geometrie-Objekt
    auto itemFor(GeoObject* geoObject) const -> GeoGraphicsItem*;
    auto geoGraphicsItems() -> std::unordered_map<GeoObject*, GeoGraphicsItem*>& { return m_map; }

    [[nodiscard]] auto intersectionSets() const -> const std::unordered_set<IntersectionSet*>& { return m_intersectionSets; }

    auto radiusPointFor(const Point* centerPoint) const -> Point*;

    [[nodiscard]] auto geoScene() const -> Scene* { return m_geoScene; }

    void select(GeoObject* geoObject);
    void deselect(GeoObject* geoObject);
    void clearSelection();
    [[nodiscard]] auto selection() const -> const std::unordered_set<GeoObject*>& { return m_selection; }

    void clear();

    void highlight(GeoObject* obj, bool isHighlighted);
    void hide(GeoObject* obj);
    void show(GeoObject* obj);
    void removeGraphicsOnly(GeoObject* obj);

    void copySelection();
    [[nodiscard]] auto clipboard() const -> std::unordered_set<GeoObject*> { return m_clipboard; };

signals:
    void selectionChanged();

private:
    Scene*          m_geoScene;
    QGraphicsScene* m_qtScene;

    std::unordered_map<GeoObject*, GeoGraphicsItem*> m_map;
    std::unordered_set<GeoObject*> m_selection;
    std::unordered_set<IntersectionSet*> m_intersectionSets;
    std::unordered_set<GeoObject*> m_clipboard;

};
