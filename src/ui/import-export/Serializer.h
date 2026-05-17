#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class Scene;
class SceneAdapter;
class GeoObject;

class Serializer {
public:
    Serializer(Scene* scene, SceneAdapter* adapter);

    // JSON Save/Load
    bool save(const QString& filename) const;
    bool load(const QString& filename);

    // SVG Export
    bool exportSVG(const QString& filename) const;

    QString lastError() const { return m_lastError; }

private:
    // Save
    QJsonObject serializeScene()      const;
    QJsonObject serializeObject(GeoObject* obj, int id, const std::unordered_map<GeoObject*, int>& idMap) const;

    // Load
    bool deserializeScene(const QJsonArray& objects);

    // SVG
    QString buildSVG() const;

    Scene*        m_scene;
    SceneAdapter* m_adapter;
    mutable QString m_lastError;
};