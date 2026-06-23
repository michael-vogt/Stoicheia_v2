#pragma once
#include <QGraphicsScene>
#include <map>
#include "../geometry/Scene.h"
#include "GeoGraphicsItem.h"
#include "GeoPointItem.h"
#include "GeoLinearObjectItem.h"
#include "GeoCircleItem.h"
#include "GeoIntersectionPointItem.h"
#include "Structs.h"
#include "constructions/IntersectionSet.h"
#include "dialogs/AppSettings.h"
#include "../Constants.h"


using namespace Constants;

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
    auto addPoint(Point* point, const QPen& pen = QPen(AppSettings::instance().colors.point, DrawingConstants::PENWIDTH_NORMAL)) -> GeoPointItem*;
    auto addLinearObject(LinearObject* linearObject) -> GeoLinearObjectItem*;
    auto addCircle(Circle* circle) -> GeoCircleItem*;
    auto addIntersectionPoint(IntersectionPoint* point, const QPen& pen = QPen(AppSettings::instance().colors.construction, DrawingConstants::PENWIDTH_NORMAL)) -> GeoIntersectionPointItem*;
    auto addIntersectionSet(IntersectionSet* intersectionSet) -> std::pair<GeoPointItem*, GeoPointItem*>;

    // Geometrie-Objekt und zugehöriges grafisches Objekt entfernen
    void remove(GeoObject* geoObject);

    // Zugriff auf das grafische Objekt zu einem Geometrie-Objekt
    auto itemFor(GeoObject* geoObject) const -> GeoGraphicsItem*;
    auto geoGraphicsItems() -> std::map<GeoObject*, GeoGraphicsItem*>& { return m_map; }

    [[nodiscard]] auto intersectionSets() const -> const std::set<IntersectionSet*>& { return m_intersectionSets; }

    auto radiusPointFor(const Point* centerPoint) const -> Point*;
    [[nodiscard]] auto geoScene() const -> Scene* { return m_geoScene; }

    void select(GeoObject* geoObject);
    void deselect(GeoObject* geoObject);
    void clearSelection();
    [[nodiscard]] auto selection() const -> const std::set<GeoObject*>& { return m_selection; }

    void clear();
    void highlight(GeoObject* obj, bool isHighlighted);
    void hide(GeoObject* obj);
    void show(GeoObject* obj);
    void setVisible(GeoObject* obj, bool visible) { visible ? show(obj) : hide(obj); }
    void setColor(GeoObject* obj, const QColor& color);
    void removeGraphicsOnly(GeoObject* obj);

    [[nodiscard]] auto colorOf(GeoObject* obj) const -> QColor;
    [[nodiscard]] auto visibleOf(GeoObject* obj) const -> bool;

    void transferStyle(GeoObjectPair pair);

    void copySelection();
    [[nodiscard]] auto clipboard() const -> std::set<GeoObject*> { return m_clipboard; };

signals:
    void selectionChanged();

private:
    struct ObjectStyle {
        QColor color;
        bool visible = true;
    };

    void applyStyle(GeoObject* obj, GeoGraphicsItem* item) const;

    Scene*          m_geoScene;
    QGraphicsScene* m_qtScene;

    std::map<GeoObject*, GeoGraphicsItem*> m_map;
    std::map<GeoObject*, ObjectStyle> m_styles;
    std::set<GeoObject*> m_selection;
    std::set<IntersectionSet*> m_intersectionSets;
    std::set<GeoObject*> m_clipboard;

};
