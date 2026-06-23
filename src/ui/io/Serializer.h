#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <map>
#include <set>
#include <vector>


class Scene;
class SceneAdapter;
class GeoObject;


class Serializer {
public:
    Serializer(Scene* scene, SceneAdapter* adapter);

    // JSON Save/Load
    auto save(const QString& filename) const -> bool;
    auto load(const QString& filename) -> bool;

    // SVG Export
    auto exportSVG(const QString& filename) const -> bool;

    auto lastError() const -> QString { return m_lastError; }

private:
    // Save
    auto serializeScene() const -> QJsonObject;
    auto serializeObject(GeoObject* obj, unsigned int ident, const std::map<GeoObject*, unsigned int>& idMap) const -> QJsonObject;
    void serializeDisplayProperties(GeoObject* obj, QJsonObject& jsonObj) const;

    // Load
    auto deserializeScene(const QJsonArray& objects) -> bool;
    auto deserializeObject(const QJsonValueConstRef& val, std::map<int, GeoObject*>& idMap) -> bool;
    static auto ref(const QString& key, const QJsonObject& jsonObj, const std::map<int, GeoObject*>& idMap) -> GeoObject*;

    // SVG
    auto buildSVG() const -> QString;

    auto buildDependencyOrder(const std::set<GeoObject*>& saveable) const -> std::vector<GeoObject*>;
    static auto buildIdMap(const std::vector<GeoObject*>& sorted)  -> std::map<GeoObject*, unsigned int>;
    void collectDependencies(GeoObject* obj, std::set<GeoObject*>& visited, std::vector<GeoObject*>& sorted, const std::set<GeoObject*>& saveable) const;
    auto collectSaveableObjects() const -> std::set<GeoObject*>;

    Scene*        m_scene;
    SceneAdapter* m_adapter;
    mutable QString m_lastError;
};