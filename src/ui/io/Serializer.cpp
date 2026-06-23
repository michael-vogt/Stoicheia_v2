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
#include <map>
#include <set>
#include <vector>

static constexpr int file_version = 1;
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
    auto id_map = buildIdMap(sorted);

    // Serialisieren
    QJsonArray objects;
    for (std::size_t i = 0; i < sorted.size(); ++i) {
        objects.append(serializeObject(sorted[i], static_cast<unsigned int>(i), id_map));
    }

    QJsonObject root;
    root["version"] = file_version;
    root["objects"] = objects;
    return root;
}

auto Serializer::serializeObject(GeoObject* obj, unsigned int ident, const std::map<GeoObject*, unsigned int>& idMap) const -> QJsonObject {
    auto ref = [&](GeoObject* obj) -> unsigned int {
        auto iter = idMap.find(obj);
        return iter != idMap.end() ? iter->second : -1;
    };

    QJsonObject json_obj;
    json_obj["id"] = static_cast<int>(ident);

    if (auto* intersection_point = dynamic_cast<IntersectionPoint*>(obj)) {
        json_obj["type"] = "IntersectionPoint";
        for (IntersectionSet* iset : m_adapter->intersectionSets()) {
            if (iset->first() == intersection_point) {
                json_obj["intersectionSet"] = static_cast<int>(ref(iset));
                json_obj["index"] = 0;
                break;
            }
            if (iset->second() == intersection_point) {
                json_obj["intersectionSet"] = static_cast<int>(ref(iset));
                json_obj["index"] = 1;
                break;
            }
        }
    } else if (auto* point = dynamic_cast<Point*>(obj)) {
        json_obj["type"] = "Point";
        json_obj["x"]    = point->x();
        json_obj["y"]    = point->y();
    } else if (auto* line = dynamic_cast<Line*>(obj)) {
        json_obj["type"] = "Line";
        json_obj["p1"]   = static_cast<int>(ref(line->point1()));
        json_obj["p2"]   = static_cast<int>(ref(line->point2()));
    } else if (auto* ray = dynamic_cast<Ray*>(obj)) {
        json_obj["type"] = "Ray";
        json_obj["p1"]   = static_cast<int>(ref(ray->point1()));
        json_obj["p2"]   = static_cast<int>(ref(ray->point2()));
    } else if (auto* segment = dynamic_cast<Segment*>(obj)) {
        json_obj["type"] = "Segment";
        json_obj["p1"]   = static_cast<int>(ref(segment->point1()));
        json_obj["p2"]   = static_cast<int>(ref(segment->point2()));
    } else if (auto* circle = dynamic_cast<Circle*>(obj)) {
        json_obj["type"]   = "Circle";
        json_obj["center"] = static_cast<int>(ref(circle->center()));
        json_obj["radius"] = static_cast<int>(ref(circle->radiusPoint()));
    } else if (auto* midpoint = dynamic_cast<Midpoint*>(obj)) {
        json_obj["type"] = "Midpoint";
        json_obj["p1"]   = static_cast<int>(ref(midpoint->point1()));
        json_obj["p2"]   = static_cast<int>(ref(midpoint->point2()));
    } else if (auto* para = dynamic_cast<Parallel*>(obj)) {
        json_obj["type"]      = "Parallel";
        json_obj["origin"]    = static_cast<int>(ref(para->origin()));
        json_obj["reference"] = static_cast<int>(ref(para->reference()));
    } else if (auto* perp = dynamic_cast<Perpendicular*>(obj)) {
        json_obj["type"]      = "Perpendicular";
        json_obj["origin"]    = static_cast<int>(ref(perp->origin()));
        json_obj["reference"] = static_cast<int>(ref(perp->reference()));
    } else if (auto* perp_foot = dynamic_cast<PerpendicularFoot*>(obj)) {
        json_obj["type"]  = "PerpendicularFoot";
        json_obj["point"] = static_cast<int>(ref(perp_foot->point()));
        json_obj["line"]  = static_cast<int>(ref(perp_foot->line()));
    } else if (auto* lli = dynamic_cast<LineLineIntersection*>(obj)) {
        json_obj["type"] = "LineLineIntersection";
        json_obj["l1"]   = static_cast<int>(ref(lli->line1()));
        json_obj["l2"]   = static_cast<int>(ref(lli->line2()));
    } else if (auto* lci = dynamic_cast<LineCircleIntersection*>(obj)) {
        json_obj["type"]   = "LineCircleIntersection";
        json_obj["line"]   = static_cast<int>(ref(lci->line()));
        json_obj["circle"] = static_cast<int>(ref(lci->circle()));
    } else if (auto* cci = dynamic_cast<CircleCircleIntersection*>(obj)) {
        json_obj["type"] = "CircleCircleIntersection";
        json_obj["c1"]   = static_cast<int>(ref(cci->circle1()));
        json_obj["c2"]   = static_cast<int>(ref(cci->circle2()));
    }

    serializeDisplayProperties(obj, json_obj);

    return json_obj;
}

void Serializer::serializeDisplayProperties(GeoObject* obj, QJsonObject& jsonObj) const {
    // Darstellungseigenschaften (Farbe, Sichtbarkeit) mitspeichern. IntersectionPoints haben kein eigenes Item
    // (Member von IntersectionSet), daher nur speichern wenn ein Item existiert.
    if (auto* item = m_adapter->itemFor(obj)) {
        QPen pen;
        bool has_pen = false;
        if (auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            pen = point_item->pen();
            has_pen = true;
        } else if (auto* linear_item = dynamic_cast<GeoLinearObjectItem*>(item)) {
            pen = linear_item->pen();
            has_pen = true;
        } else if (auto* circle_item = dynamic_cast<GeoCircleItem*>(item)) {
            pen = circle_item->pen();
            has_pen = true;
        }

        if (has_pen) {
            jsonObj["color"] = pen.color().name(QColor::HexArgb);
        }
        jsonObj["visible"] = item->isVisible();
    }
}

// ── Load ─────────────────────────────────────────────────────────────────────

auto Serializer::load(const QString& filename) -> bool {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = file.errorString();
        return false;
    }

    QJsonParseError parse_error;
    auto doc = QJsonDocument::fromJson(file.readAll(), &parse_error);
    if (parse_error.error != QJsonParseError::NoError) {
        m_lastError = parse_error.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    if (root["version"].toInt() > file_version) {
        m_lastError = "Dateiversion wird nicht unterstützt";
        return false;
    }

    return deserializeScene(root["objects"].toArray());
}

auto Serializer::deserializeScene(const QJsonArray& objects) -> bool {
    // Zwei Durchläufe:
    // 1. Alle Objekte erzeugen (damit Referenzen aufgelöst werden können)
    // 2. Referenzen auflösen und grafische Objekte anlegen

    std::map<int, GeoObject*> id_map;

    for (const auto& val : objects) {
        if (!deserializeObject(val, id_map)) {
            return false;
        }
    }
    return true;
}

auto Serializer::ref(const QString& key, const QJsonObject& jsonObj, const std::map<int, GeoObject*>& idMap) -> GeoObject* {
    int ref_id = jsonObj[key].toInt(-1);
    auto iter = idMap.find(ref_id);
    return (iter != idMap.end()) ? iter->second : nullptr;
}

auto Serializer::deserializeObject(const QJsonValueConstRef& val, std::map<int, GeoObject*>& idMap) -> bool { // NOLINT
    QJsonObject json_obj = val.toObject();
    int         ident   = json_obj["id"].toInt();
    QString     type    = json_obj["type"].toString();

    GeoObject* obj = nullptr;

    if (type == "IntersectionPoint") {
        int set_id  = json_obj["intersectionSet"].toInt(-1);
        int index   = json_obj["index"].toInt(0);
        auto iter = idMap.find(set_id);
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
        auto* point = m_scene->create<Point>(json_obj["x"].toDouble(), json_obj["y"].toDouble());
        m_adapter->addPoint(point);
        obj = point;
    } else if (type == "Line") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", json_obj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", json_obj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) { 
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* line = m_scene->create<Line>(PointPairForLinearObject{.point1=point1, .point2=point2});
        m_adapter->addLinearObject(line);
        obj = line;

    } else if (type == "Ray") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", json_obj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", json_obj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) { 
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* ray = m_scene->create<Ray>(PointPairForLinearObject{.point1=point1, .point2=point2});
        m_adapter->addLinearObject(ray);
        obj = ray;

    } else if (type == "Segment") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", json_obj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", json_obj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* segment = m_scene->create<Segment>(PointPairForLinearObject{.point1=point1, .point2=point2});
        m_adapter->addLinearObject(segment);
        obj = segment;

    } else if (type == "Circle") {
        auto* center = dynamic_cast<Point*>(ref("center", json_obj, idMap));
        auto* radius = dynamic_cast<Point*>(ref("radius", json_obj, idMap));
        if ((center == nullptr) || (radius == nullptr)) { 
            m_lastError = "Ungültige Referenz"; 
            return false; 
        }
        auto* circle = m_scene->create<Circle>(PointPairForCircle{.center=center,.radiusPoint=radius});
        m_adapter->addCircle(circle);
        obj = circle;

    } else if (type == "Midpoint") {
        auto* point1 = dynamic_cast<Point*>(ref("p1", json_obj, idMap));
        auto* point2 = dynamic_cast<Point*>(ref("p2", json_obj, idMap));
        if ((point1 == nullptr) || (point2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* midpoint = m_scene->create<Midpoint>(point1, point2);
        m_adapter->addPoint(midpoint);
        obj = midpoint;

    } else if (type == "Parallel") {
        auto* origin = dynamic_cast<Point*>(ref("origin", json_obj, idMap));
        auto* ref_obj = dynamic_cast<LinearObject*>(ref("reference", json_obj, idMap));
        if ((origin == nullptr) || (ref_obj == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* para = m_scene->create<Parallel>(origin, ref_obj);
        m_adapter->addLinearObject(para->line());
        obj = para;

    } else if (type == "Perpendicular") {
        auto* origin = dynamic_cast<Point*>(ref("origin", json_obj, idMap));
        auto* ref_obj = dynamic_cast<LinearObject*>(ref("reference", json_obj, idMap));
        if ((origin == nullptr) || (ref_obj == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* perp = m_scene->create<Perpendicular>(origin, ref_obj);
        m_adapter->addLinearObject(perp->line());
        obj = perp;

    } else if (type == "PerpendicularFoot") {
        auto* point = dynamic_cast<Point*>(ref("point", json_obj, idMap));
        auto* line  = dynamic_cast<LinearObject*>(ref("line", json_obj, idMap));
        if ((point == nullptr) || (line == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* perp_foot = m_scene->create<PerpendicularFoot>(point, line);
        m_adapter->addPoint(perp_foot);
        obj = perp_foot;

    } else if (type == "LineLineIntersection") {
        auto* line1 = dynamic_cast<LinearObject*>(ref("l1", json_obj, idMap));
        auto* line2 = dynamic_cast<LinearObject*>(ref("l2", json_obj, idMap));
        if ((line1 == nullptr) || (line2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* lli = m_scene->create<LineLineIntersection>(line1, line2);
        m_adapter->addIntersectionSet(lli);
        obj = lli;

    } else if (type == "LineCircleIntersection") {
        auto* line   = dynamic_cast<LinearObject*>(ref("line", json_obj, idMap));
        auto* circle = dynamic_cast<Circle*>(ref("circle", json_obj, idMap));
        if ((line == nullptr) || (circle == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* lci = m_scene->create<LineCircleIntersection>(line, circle);
        m_adapter->addIntersectionSet(lci);
        obj = lci;

    } else if (type == "CircleCircleIntersection") {
        auto* circle1 = dynamic_cast<Circle*>(ref("c1", json_obj, idMap));
        auto* circle2 = dynamic_cast<Circle*>(ref("c2", json_obj, idMap));
        if ((circle1 == nullptr) || (circle2 == nullptr)) {
            m_lastError = "Ungültige Referenz";
            return false; 
        }
        auto* cci = m_scene->create<CircleCircleIntersection>(circle1, circle2);
        m_adapter->addIntersectionSet(cci);
        obj = cci;
    }

    if (obj != nullptr) {
        // Darstellungseigenschaften (Farbe, Sichtbarkeit) wiederherstellen.
        // IntersectionPoints haben kein eigenes Item, daher Existenz prüfen.
        if (auto* item = m_adapter->itemFor(obj)) {
            if (json_obj.contains("color")) {
                QColor color(json_obj["color"].toString());
                if (color.isValid()) {
                    m_adapter->setColor(obj, color);
                    /*if (auto* pi = dynamic_cast<GeoPointItem*>(item)) {
                        QPen pen = pi->pen();
                        pen.setColor(color);
                        pi->setPen(pen);
                    } else if (auto* li = dynamic_cast<GeoLinearObjectItem*>(item)) {
                        QPen pen = li->pen();
                        pen.setColor(color);
                        li->setPen(pen);
                    } else if (auto* ci = dynamic_cast<GeoCircleItem*>(item)) {
                        QPen pen = ci->pen();
                        pen.setColor(color);
                        ci->setPen(pen);
                    }*/
                }
            }
            if (json_obj.contains("visible")) {
                bool visible = json_obj["visible"].toBool(true);
                visible ? m_adapter->show(obj) : m_adapter->hide(obj);
            }
        }
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
    double min_x =  SVG::BOUNDINGBOX_WIDTH;
    double min_y =  SVG::BOUNDINGBOX_WIDTH;
    double max_x = -SVG::BOUNDINGBOX_WIDTH;
    double max_y = -SVG::BOUNDINGBOX_WIDTH;

    const auto& items = m_adapter->geoGraphicsItems();

    auto expand = [&](double pos_x, double pos_y) -> void {
        min_x = std::min(min_x, pos_x); min_y = std::min(min_y, pos_y);
        max_x = std::max(max_x, pos_x); max_y = std::max(max_y, pos_y);
    };

    for (const auto& [obj, item] : items) {
        if (!item->isVisible() || !obj->isValid()) {
            continue;
        }
        if (auto* point = dynamic_cast<Point*>(obj)) {
            expand(point->x(), point->y());
        }
    }

    if (min_x > max_x) { 
        min_x = -SVG::MINSIZE;
        max_x = SVG::MINSIZE;
        min_y = -SVG::MINSIZE;
        max_y = SVG::MINSIZE; 
    }
    min_x -= SVG::MARGIN;
    min_y -= SVG::MARGIN;
    max_x += SVG::MARGIN;
    max_y += SVG::MARGIN;

    double width = max_x - min_x;
    double height = max_y - min_y;

    // SVG-Koordinaten: Y-Achse invertieren (SVG hat Y nach unten)
    auto svg_x = [&](double pos_x) -> double { return pos_x - min_x; };
    auto svg_y = [&](double pos_y) -> double { return height - (pos_y - min_y); }; // Y-Flip

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

        if (auto* linear_object = dynamic_cast<LinearObject*>(obj)) {
            double delta_x = linear_object->dx();
            double delta_y = linear_object->dy();
            double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));
            if (len < eps) {
                continue;
            }
            double dir_x = delta_x/len;
            double dir_y = delta_y/len;

            double p1_x = linear_object->point1()->x();
            double p1_y = linear_object->point1()->y();
            double p2_x = linear_object->point2()->x();
            double p2_y = linear_object->point2()->y();
            if (dynamic_cast<Ray*>(linear_object) != nullptr) {
                p2_x += (dir_x*SVG::LINE_EXTENT);
                p2_y += (dir_y*SVG::LINE_EXTENT);
            } else if (dynamic_cast<Line*>(linear_object) != nullptr) {
                p1_x -= (dir_x*SVG::LINE_EXTENT);
                p1_y -= (dir_y*SVG::LINE_EXTENT);
                p2_x += (dir_x*SVG::LINE_EXTENT);
                p2_y += (dir_y*SVG::LINE_EXTENT);
            }

            svg += QString(R"(  <line x1="%1" y1="%2" x2="%3" y2="%4"
        stroke="black" stroke-width="1.5"/>
)")
                .arg(svg_x(p1_x)).arg(svg_y(p1_y))
                .arg(svg_x(p2_x)).arg(svg_y(p2_y));

        } else if (auto* circle = dynamic_cast<Circle*>(obj)) {
            svg += QString(R"(  <circle cx="%1" cy="%2" r="%3"
        fill="none" stroke="black" stroke-width="1.5"/>
)")
                .arg(svg_x(circle->center()->x()))
                .arg(svg_y(circle->center()->y()))
                .arg(circle->radius());

        } else if (auto* point = dynamic_cast<Point*>(obj)) {
            svg += QString(R"(  <circle cx="%1" cy="%2" r="4"
        fill="white" stroke="black" stroke-width="1.5"/>
)")
                .arg(svg_x(point->x()))
                .arg(svg_y(point->y()));
        }
    }

    svg += "</svg>\n";
    return svg;
}

auto Serializer::buildDependencyOrder(const std::set<GeoObject*>& saveable) const -> std::vector<GeoObject*> {
    // Topologisch sortieren – nur saveables
    std::vector<GeoObject*> sorted;
    std::set<GeoObject*> visited;

    for (GeoObject* obj : saveable) {
        collectDependencies(obj, visited, sorted, saveable);
    }

    return sorted;
}

auto Serializer::buildIdMap(const std::vector<GeoObject*>& sorted) -> std::map<GeoObject*, unsigned int> {
    std::map<GeoObject*, unsigned int> id_map;
    // IDs in topologischer Reihenfolge vergeben
    // IntersectionPoints bekommen auch IDs (für Referenzen aus anderen Objekten)
    for (std::size_t i = 0; i < sorted.size(); ++i) {
        id_map[sorted[i]] = static_cast<unsigned int>(i);
    }

    // IntersectionPoints der gespeicherten IntersectionSets mit IDs versehen
    int next_id = static_cast<int>(sorted.size());
    for (GeoObject* obj : sorted) {
        if (auto* iset = dynamic_cast<IntersectionSet*>(obj)) {
            if (!id_map.contains(iset->first())) {
                id_map[iset->first()]  = next_id++;
            }
            if (!id_map.contains(iset->second())) {
                id_map[iset->second()] = next_id++;
            }
        }
    }

    return id_map;
}

void Serializer::collectDependencies(GeoObject* obj, std::set<GeoObject*>& visited, std::vector<GeoObject*>& sorted, const std::set<GeoObject*>& saveable) const {
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
        if (auto* intersection_point = dynamic_cast<IntersectionPoint*>(obj)) {
            for (auto& [geoObj, unused] : m_adapter->geoGraphicsItems()) {
                if (auto* iset = dynamic_cast<IntersectionSet*>(geoObj)) {
                    if (iset->first() == intersection_point || iset->second() == intersection_point) {
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

auto Serializer::collectSaveableObjects() const -> std::set<GeoObject*> {
    const auto& items = m_adapter->geoGraphicsItems();
    // Alle validen Objekte speichern - auch ausgeblendete (Sichtbarkeit wird separat als Attribut "visible"
    // mitgespeichert - siehe serisalizeObject())
    std::set<GeoObject*> saveable;
    for (const auto& [obj, item] : items) {
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