#include "Serializer.h"
#include "../SceneAdapter.h"
#include "../../geometry/Point.h"
#include "../../geometry/Line.h"
#include "../../geometry/Ray.h"
#include "../../geometry/Segment.h"
#include "../../geometry/Circle.h"
#include "../../constructions/Midpoint.h"
#include "../../constructions/Parallel.h"
#include "../../constructions/Perpendicular.h"
#include "../../constructions/PerpendicularFoot.h"
#include "../../constructions/LineLineIntersection.h"
#include "../../constructions/LineCircleIntersection.h"
#include "../../constructions/CircleCircleIntersection.h"
#include "Structs.h"
#include "geometry/GeoObject.h"
#include "../../Constants.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <qjsonobject.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

static constexpr int FILE_VERSION = 1;
//constexpr double eps = std::numeric_limits<double>::epsilon();

using namespace Constants::ExportConstants;


Serializer::Serializer(Scene* scene, SceneAdapter* adapter)
    : m_scene(scene), m_adapter(adapter)
{}

// ── Save ─────────────────────────────────────────────────────────────────────

auto Serializer::save(const QString& filename) const -> bool {
    QJsonDocument doc(serializeScene());
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = file.errorString();
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

auto Serializer::serializeScene() const -> QJsonObject {
    const auto& items = m_adapter->geoGraphicsItems();

    auto saveable = collectSaveableObjects();
    auto sorted = buildDependencyOrder(saveable);
    auto idMap = buildIdMap(sorted);

    // Serialisieren
    QJsonArray objects;
    for (int i = 0; i < static_cast<int>(sorted.size()); ++i) {
        objects.append(serializeObject(sorted[i], i, idMap));
    }

    QJsonObject root;
    root["version"] = FILE_VERSION;
    root["objects"] = objects;
    return root;
}

auto Serializer::serializeObject(GeoObject* obj, int ident, const std::unordered_map<GeoObject*, int>& idMap) const -> QJsonObject {
    auto ref = [&](GeoObject* obj) -> int {
        auto iter = idMap.find(obj);
        return iter != idMap.end() ? iter->second : -1;
    };

    QJsonObject jsobObj;
    jsobObj["id"] = ident;

    if (auto* intersectionPoint = dynamic_cast<IntersectionPoint*>(obj)) {
        jsobObj["type"] = "IntersectionPoint";
        for (IntersectionSet* iset : m_adapter->intersectionSets()) {
            if (iset->first() == intersectionPoint) {
                jsobObj["intersectionSet"] = ref(iset);
                jsobObj["index"] = 0;
                break;
            }
            if (iset->second() == intersectionPoint) {
                jsobObj["intersectionSet"] = ref(iset);
                jsobObj["index"] = 1;
                break;
            }
        }
    } else if (auto* point = dynamic_cast<Point*>(obj)) {
        jsobObj["type"] = "Point";
        jsobObj["x"]    = point->x();
        jsobObj["y"]    = point->y();
    } else if (auto* line = dynamic_cast<Line*>(obj)) {
        jsobObj["type"] = "Line";
        jsobObj["p1"]   = ref(line->point1());
        jsobObj["p2"]   = ref(line->point2());
    } else if (auto* ray = dynamic_cast<Ray*>(obj)) {
        jsobObj["type"] = "Ray";
        jsobObj["p1"]   = ref(ray->point1());
        jsobObj["p2"]   = ref(ray->point2());
    } else if (auto* segment = dynamic_cast<Segment*>(obj)) {
        jsobObj["type"] = "Segment";
        jsobObj["p1"]   = ref(segment->point1());
        jsobObj["p2"]   = ref(segment->point2());
    } else if (auto* circle = dynamic_cast<Circle*>(obj)) {
        jsobObj["type"]   = "Circle";
        jsobObj["center"] = ref(circle->center());
        jsobObj["radius"] = ref(circle->radiusPoint());
    } else if (auto* midpoint = dynamic_cast<Midpoint*>(obj)) {
        jsobObj["type"] = "Midpoint";
        jsobObj["p1"]   = ref(midpoint->point1());
        jsobObj["p2"]   = ref(midpoint->point2());
    } else if (auto* para = dynamic_cast<Parallel*>(obj)) {
        jsobObj["type"]      = "Parallel";
        jsobObj["origin"]    = ref(para->origin());
        jsobObj["reference"] = ref(para->reference());
    } else if (auto* perp = dynamic_cast<Perpendicular*>(obj)) {
        jsobObj["type"]      = "Perpendicular";
        jsobObj["origin"]    = ref(perp->origin());
        jsobObj["reference"] = ref(perp->reference());
    } else if (auto* perp_foot = dynamic_cast<PerpendicularFoot*>(obj)) {
        jsobObj["type"]  = "PerpendicularFoot";
        jsobObj["point"] = ref(perp_foot->point());
        jsobObj["line"]  = ref(perp_foot->line());
    } else if (auto* lli = dynamic_cast<LineLineIntersection*>(obj)) {
        jsobObj["type"] = "LineLineIntersection";
        jsobObj["l1"]   = ref(lli->line1());
        jsobObj["l2"]   = ref(lli->line2());
    } else if (auto* lci = dynamic_cast<LineCircleIntersection*>(obj)) {
        jsobObj["type"]   = "LineCircleIntersection";
        jsobObj["line"]   = ref(lci->line());
        jsobObj["circle"] = ref(lci->circle());
    } else if (auto* cci = dynamic_cast<CircleCircleIntersection*>(obj)) {
        jsobObj["type"] = "CircleCircleIntersection";
        jsobObj["c1"]   = ref(cci->circle1());
        jsobObj["c2"]   = ref(cci->circle2());
    }

    return jsobObj;
}

// ── Load ─────────────────────────────────────────────────────────────────────

auto Serializer::load(const QString& filename) -> bool {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = file.errorString();
        return false;
    }

    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        m_lastError = parseError.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    if (root["version"].toInt() > FILE_VERSION) {
        m_lastError = "Dateiversion wird nicht unterstützt";
        return false;
    }

    return deserializeScene(root["objects"].toArray());
}

auto Serializer::deserializeScene(const QJsonArray& objects) -> bool {
    // Zwei Durchläufe:
    // 1. Alle Objekte erzeugen (damit Referenzen aufgelöst werden können)
    // 2. Referenzen auflösen und grafische Objekte anlegen

    std::unordered_map<int, GeoObject*> idMap;

    for (const auto& val : objects) {
        if (!deserializeObject(val, idMap)) {
            return false;
        }
    }
    return true;
}

auto Serializer::ref(const QString& key, const QJsonObject& jsonObj, const std::unordered_map<int, GeoObject*>& idMap) -> GeoObject* {
    int refId = jsonObj[key].toInt(-1);
    auto iter = idMap.find(refId);
    return (iter != idMap.end()) ? iter->second : nullptr;
}

auto Serializer::deserializeObject(const QJsonValueConstRef& val, std::unordered_map<int, GeoObject*>& idMap) -> bool { // NOLINT
    QJsonObject jsonObj = val.toObject();
    int         ident   = jsonObj["id"].toInt();
    QString     type    = jsonObj["type"].toString();

    GeoObject* obj = nullptr;

    if (type == "IntersectionPoint") {
        int setId  = jsonObj["intersectionSet"].toInt(-1);
        int index  = jsonObj["index"].toInt(0);
        auto iter = idMap.find(setId);
        if (iter == idMap.end()) {
            m_lastError = "IntersectionSet nicht gefunden";
            return false;
        }
        auto* iset = dynamic_cast<IntersectionSet*>(iter->second);
        if (iset == nullptr) { 
            m_lastError = "Kein IntersectionSet";
            return false;
        }
        obj = (index == 0) ? iset->first() : iset->second();
        // Kein addPoint nötig – Item wurde bereits von addIntersectionSet erzeugt
        idMap[ident] = obj;
        //continue; // kein erneutes addPoint
    } 
    
    if (type == "Point") {
        auto* point = m_scene->create<Point>(jsonObj["x"].toDouble(), jsonObj["y"].toDouble());
        m_adapter->addPoint(point);
        obj = point;
    } else if (type == "Line") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", jsonObj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", jsonObj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) { 
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* line = m_scene->create<Line>(PointPairForLinearObject{.point1=point1, .point2=point2});
        m_adapter->addLinearObject(line);
        obj = line;

    } else if (type == "Ray") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", jsonObj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", jsonObj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) { 
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* ray = m_scene->create<Ray>(PointPairForLinearObject{.point1=point1, .point2=point2});
        m_adapter->addLinearObject(ray);
        obj = ray;

    } else if (type == "Segment") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", jsonObj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", jsonObj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* segment = m_scene->create<Segment>(PointPairForLinearObject{.point1=point1, .point2=point2});
        m_adapter->addLinearObject(segment);
        obj = segment;

    } else if (type == "Circle") {
        auto* center = dynamic_cast<Point*>(ref("center", jsonObj, idMap));
        auto* radius = dynamic_cast<Point*>(ref("radius", jsonObj, idMap));
        if ((center == nullptr) || (radius == nullptr)) { 
            m_lastError = "Ungültige Referenz"; 
            return false; 
        }
        auto* circle = m_scene->create<Circle>(PointPairForCircle{.center=center,.radiusPoint=radius});
        m_adapter->addCircle(circle);
        obj = circle;

    } else if (type == "Midpoint") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", jsonObj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", jsonObj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* midpoint = m_scene->create<Midpoint>(point1, point2);
        m_adapter->addPoint(midpoint);
        obj = midpoint;

    } else if (type == "Parallel") {
        auto* origin = dynamic_cast<Point*>(ref("origin", jsonObj, idMap));
        auto* refObj = dynamic_cast<LinearObject*>(ref("reference", jsonObj, idMap));
        if ((origin == nullptr) || (refObj == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* para = m_scene->create<Parallel>(origin, refObj);
        m_adapter->addLinearObject(para->line());
        obj = para;

    } else if (type == "Perpendicular") {
        auto* origin = dynamic_cast<Point*>(ref("origin", jsonObj, idMap));
        auto* refObj = dynamic_cast<LinearObject*>(ref("reference", jsonObj, idMap));
        if ((origin == nullptr) || (refObj == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* perp = m_scene->create<Perpendicular>(origin, refObj);
        m_adapter->addLinearObject(perp->line());
        obj = perp;

    } else if (type == "PerpendicularFoot") {
        auto* point = dynamic_cast<Point*>(ref("point", jsonObj, idMap));
        auto* line  = dynamic_cast<LinearObject*>(ref("line", jsonObj, idMap));
        if ((point == nullptr) || (line == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* perp_foot = m_scene->create<PerpendicularFoot>(point, line);
        m_adapter->addPoint(perp_foot);
        obj = perp_foot;

    } else if (type == "LineLineIntersection") {
        auto* line1 = dynamic_cast<LinearObject*>(ref("l1", jsonObj, idMap));
        auto* line2 = dynamic_cast<LinearObject*>(ref("l2", jsonObj, idMap));
        if ((line1 == nullptr) || (line2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* lli = m_scene->create<LineLineIntersection>(line1, line2);
        m_adapter->addIntersectionSet(lli);
        obj = lli;

    } else if (type == "LineCircleIntersection") {
        auto* line   = dynamic_cast<LinearObject*>(ref("line", jsonObj, idMap));
        auto* circle = dynamic_cast<Circle*>(ref("circle", jsonObj, idMap));
        if ((line == nullptr) || (circle == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* lci = m_scene->create<LineCircleIntersection>(line, circle);
        m_adapter->addIntersectionSet(lci);
        obj = lci;

    } else if (type == "CircleCircleIntersection") {
        auto* circle1 = dynamic_cast<Circle*>(ref("c1", jsonObj, idMap));
        auto* circle2 = dynamic_cast<Circle*>(ref("c2", jsonObj, idMap));
        if ((circle1 == nullptr) || (circle2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* cci = m_scene->create<CircleCircleIntersection>(circle1, circle2);
        m_adapter->addIntersectionSet(cci);
        obj = cci;
    }

    if (obj != nullptr) {
        idMap[ident] = obj;
    }

    return true;
}

// ── SVG Export ────────────────────────────────────────────────────────────────

auto Serializer::exportSVG(const QString& filename) const -> bool {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = file.errorString();
        return false;
    }
    file.write(buildSVG().toUtf8());
    return true;
}

auto Serializer::buildSVG() const -> QString {
    // Bounding box aus allen Objekten bestimmen
    double minX =  SVG::BOUNDINGBOX_WIDTH;
    double minY =  SVG::BOUNDINGBOX_WIDTH;
    double maxX = -SVG::BOUNDINGBOX_WIDTH;
    double maxY = -SVG::BOUNDINGBOX_WIDTH;

    const auto& items = m_adapter->geoGraphicsItems();

    auto expand = [&](double pos_x, double pos_y) -> void {
        minX = std::min(minX, pos_x); minY = std::min(minY, pos_y);
        maxX = std::max(maxX, pos_x); maxY = std::max(maxY, pos_y);
    };

    for (const auto& [obj, item] : items) {
        if (!item->isVisible() || !obj->isValid()) {
            continue;
        }
        if (auto* point = dynamic_cast<Point*>(obj)) {
            expand(point->x(), point->y());
        }
    }

    if (minX > maxX) { 
        minX = -SVG::MINSIZE;
        maxX = SVG::MINSIZE;
        minY = -SVG::MINSIZE;
        maxY = SVG::MINSIZE; 
    }
    minX -= SVG::MARGIN;
    minY -= SVG::MARGIN;
    maxX += SVG::MARGIN;
    maxY += SVG::MARGIN;

    double width = maxX - minX;
    double height = maxY - minY;

    // SVG-Koordinaten: Y-Achse invertieren (SVG hat Y nach unten)
    auto svgX = [&](double pos_x) -> double { return pos_x - minX; };
    auto svgY = [&](double pos_y) -> double { return height - (pos_y - minY); }; // Y-Flip

    QString svg;
    svg += QString(R"(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"
     width="%1" height="%2"
     viewBox="0 0 %1 %2">
<rect width="%1" height="%2" fill="white"/>
)").arg(width).arg(height);

    for (const auto& [obj, item] : items) {
        if (!item->isVisible() || !obj->isValid()) {
            continue;
        }

        if (auto* linearObject = dynamic_cast<LinearObject*>(obj)) {
            double delta_x = linearObject->dx();
            double delta_y = linearObject->dy();
            double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));
            if (len < eps) {
                continue;
            }
            double dir_x = delta_x/len;
            double dir_y = delta_y/len;

            double p1_x = linearObject->point1()->x();
            double p1_y = linearObject->point1()->y();
            double p2_x = linearObject->point2()->x();
            double p2_y = linearObject->point2()->y();
            if (dynamic_cast<Ray*>(linearObject) != nullptr) {
                p2_x += (dir_x*SVG::LINE_EXTENT);
                p2_y += (dir_y*SVG::LINE_EXTENT);
            } else if (dynamic_cast<Line*>(linearObject) != nullptr) {
                p1_x -= (dir_x*SVG::LINE_EXTENT);
                p1_y -= (dir_y*SVG::LINE_EXTENT);
                p2_x += (dir_x*SVG::LINE_EXTENT);
                p2_y += (dir_y*SVG::LINE_EXTENT);
            }

            svg += QString(R"(  <line x1="%1" y1="%2" x2="%3" y2="%4"
        stroke="black" stroke-width="1.5"/>
)")
                .arg(svgX(p1_x)).arg(svgY(p1_y))
                .arg(svgX(p2_x)).arg(svgY(p2_y));

        } else if (auto* circle = dynamic_cast<Circle*>(obj)) {
            svg += QString(R"(  <circle cx="%1" cy="%2" r="%3"
        fill="none" stroke="black" stroke-width="1.5"/>
)")
                .arg(svgX(circle->center()->x()))
                .arg(svgY(circle->center()->y()))
                .arg(circle->radius());

        } else if (auto* point = dynamic_cast<Point*>(obj)) {
            svg += QString(R"(  <circle cx="%1" cy="%2" r="4"
        fill="white" stroke="black" stroke-width="1.5"/>
)")
                .arg(svgX(point->x()))
                .arg(svgY(point->y()));
        }
    }

    svg += "</svg>\n";
    return svg;
}

auto Serializer::buildDependencyOrder(const std::unordered_set<GeoObject*>& saveable) const -> std::vector<GeoObject*> {
    // Topologisch sortieren – nur saveables
    std::vector<GeoObject*> sorted;
    std::unordered_set<GeoObject*> visited;

    for (GeoObject* obj : saveable) {
        collectDependencies(obj, visited, sorted, saveable);
    }

    return sorted;
}

auto Serializer::buildIdMap(const std::vector<GeoObject*>& sorted) -> std::unordered_map<GeoObject*, int> {
    std::unordered_map<GeoObject*, int> idMap;
    // IDs in topologischer Reihenfolge vergeben
    // IntersectionPoints bekommen auch IDs (für Referenzen aus anderen Objekten)
    for (int i = 0; i < static_cast<int>(sorted.size()); ++i) {
        idMap[sorted[i]] = i;
    }

    // IntersectionPoints der gespeicherten IntersectionSets mit IDs versehen
    int nextId = static_cast<int>(sorted.size());
    for (GeoObject* obj : sorted) {
        if (auto* iset = dynamic_cast<IntersectionSet*>(obj)) {
            if (!idMap.contains(iset->first())) {
                idMap[iset->first()]  = nextId++;
            }
            if (!idMap.contains(iset->second())) {
                idMap[iset->second()] = nextId++;
            }
        }
    }

    return idMap;
}

void Serializer::collectDependencies(GeoObject* obj, std::unordered_set<GeoObject*>& visited, std::vector<GeoObject*>& sorted, const std::unordered_set<GeoObject*>& saveable) const {
    if (visited.contains(obj)) {
            return;
        }
        visited.insert(obj);

        // Normale Quellen
        for (GeoObject* src : obj->sources()) {
            if (saveable.contains(src)) {
                collectDependencies(src, visited, sorted, saveable);
            }
        }

        // IntersectionPoint: IntersectionSet als implizite Quelle behandeln
        if (auto* intersectionPoint = dynamic_cast<IntersectionPoint*>(obj)) {
            for (auto& [geoObj, unused] : m_adapter->geoGraphicsItems()) {
                if (auto* iset = dynamic_cast<IntersectionSet*>(geoObj)) {
                    if (iset->first() == intersectionPoint || iset->second() == intersectionPoint) {
                        if (saveable.contains(iset)) {
                            collectDependencies(iset, visited, sorted, saveable);
                        }
                        break;
                    }
                }
            }
        }

        sorted.push_back(obj);
}

auto Serializer::collectSaveableObjects() const -> std::unordered_set<GeoObject*> {
    const auto& items = m_adapter->geoGraphicsItems();
    // Nur sichtbare, valide, nicht-IntersectionPoint Objekte speichern
    std::unordered_set<GeoObject*> saveable;
    for (const auto& [obj, item] : items) {
        if (!item->isVisible()) {
            continue;
        }
        if (!obj->isValid()) {
            continue;
        }
        //if (dynamic_cast<IntersectionPoint*>(obj)) continue;
        saveable.insert(obj);
    }

    // IntersectionSets explizit hinzufügen
    for (IntersectionSet* iset : m_adapter->intersectionSets()) {
        if (iset->isValid()) {
            saveable.insert(iset);
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (GeoObject* obj : saveable) {
            for (GeoObject* src : obj->sources()) {
                if (!saveable.contains(src) && src->isValid()) {
                    saveable.insert(src);
                    changed = true;
                }
            }
        }
    }

    return saveable;
}