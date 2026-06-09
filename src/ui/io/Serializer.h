#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <qjsonvalue.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Scene;
class SceneAdapter;
class GeoObject;

constexpr double DEFAULT_SVG_BB_SIZE = 1e10;
constexpr double DEFAULT_SVG_MARGIN = 50;
constexpr double DEFAULT_SVG_MINSIZE = 100;
constexpr double DEFAULT_SVG_LINEEXTEND = 10000;

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
    auto serializeObject(GeoObject* obj, int ident, const std::unordered_map<GeoObject*, int>& idMap) const -> QJsonObject;

    // Load
    auto deserializeScene(const QJsonArray& objects) -> bool;
    auto deserializeObject(const QJsonValueConstRef& val, std::unordered_map<int, GeoObject*>& idMap) -> bool;
    static auto ref(const QString& key, const QJsonObject& jsonObj, const std::unordered_map<int, GeoObject*>& idMap) -> GeoObject*;

    // SVG
    auto buildSVG() const -> QString;

    auto buildDependencyOrder(const std::unordered_set<GeoObject*>& saveable) const -> std::vector<GeoObject*>;
    static auto buildIdMap(const std::vector<GeoObject*>& sorted)  -> std::unordered_map<GeoObject*, int>;
    void collectDependencies(GeoObject* obj, std::unordered_set<GeoObject*>& visited, std::vector<GeoObject*>& sorted, const std::unordered_set<GeoObject*>& saveable) const;
    auto collectSaveableObjects() const -> std::unordered_set<GeoObject*>;

    Scene*        m_scene;
    SceneAdapter* m_adapter;
    mutable QString m_lastError;
};