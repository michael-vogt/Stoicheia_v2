#include "Serializer.h"
#include "../SceneAdapter.h"
#include "../../geometry/Point.h"
#include "../../geometry/Line.h"
#include "../../geometry/Ray.h"
#include "../../geometry/Segment.h"
#include "../../geometry/Circle.h"
#include "../../geometry/Triangle.h"
#include "../../constructions/Midpoint.h"
#include "../../constructions/Parallel.h"
#include "../../constructions/Perpendicular.h"
#include "../../constructions/PerpendicularFoot.h"
#include "../../constructions/LineLineIntersection.h"
#include "../../constructions/LineCircleIntersection.h"
#include "../../constructions/CircleCircleIntersection.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <unordered_map>

static constexpr int FILE_VERSION = 1;

Serializer::Serializer(Scene* scene, SceneAdapter* adapter)
    : m_scene(scene), m_adapter(adapter)
{}

// ── Save ─────────────────────────────────────────────────────────────────────

bool Serializer::save(const QString& filename) const {
    QJsonDocument doc(serializeScene());
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = file.errorString();
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

QJsonObject Serializer::serializeScene() const {
    const auto& items = m_adapter->geoGraphicsItems();

    // Nur sichtbare, valide, nicht-IntersectionPoint Objekte speichern
    std::unordered_set<GeoObject*> saveable;
    for (auto& [obj, item] : items) {
        if (!item->isVisible()) continue;
        if (!obj->isValid())    continue;
        //if (dynamic_cast<IntersectionPoint*>(obj)) continue;
        saveable.insert(obj);
    }

    // IntersectionSets explizit hinzufügen
    for (IntersectionSet* iset : m_adapter->intersectionSets()) {
        if (iset->isValid())
            saveable.insert(iset);
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

    // Topologisch sortieren – nur saveables
    std::unordered_map<GeoObject*, int> idMap;
    std::vector<GeoObject*> sorted;
    std::unordered_set<GeoObject*> visited;

    std::function<void(GeoObject*)> collect = [&](GeoObject* obj) {
        if (visited.contains(obj)) return;
        visited.insert(obj);

        // Normale Quellen
        for (GeoObject* src : obj->sources())
            if (saveable.contains(src))
                collect(src);

        // IntersectionPoint: IntersectionSet als implizite Quelle behandeln
        if (auto* ip = dynamic_cast<IntersectionPoint*>(obj)) {
            for (auto& [geoObj, _] : m_adapter->geoGraphicsItems()) {
                if (auto* iset = dynamic_cast<IntersectionSet*>(geoObj)) {
                    if (iset->first() == ip || iset->second() == ip) {
                        if (saveable.contains(iset))
                            collect(iset);
                        break;
                    }
                }
            }
        }

        sorted.push_back(obj);
    };

    for (GeoObject* obj : saveable)
        collect(obj);

    // IDs in topologischer Reihenfolge vergeben
    // IntersectionPoints bekommen auch IDs (für Referenzen aus anderen Objekten)
    for (int i = 0; i < static_cast<int>(sorted.size()); ++i)
        idMap[sorted[i]] = i;

    // IntersectionPoints der gespeicherten IntersectionSets mit IDs versehen
    int nextId = static_cast<int>(sorted.size());
    for (GeoObject* obj : sorted) {
        if (auto* iset = dynamic_cast<IntersectionSet*>(obj)) {
            if (!idMap.contains(iset->first()))
                idMap[iset->first()]  = nextId++;
            if (!idMap.contains(iset->second()))
                idMap[iset->second()] = nextId++;
        }
    }

    // Serialisieren
    QJsonArray objects;
    for (int i = 0; i < static_cast<int>(sorted.size()); ++i)
        objects.append(serializeObject(sorted[i], i, idMap));

    QJsonObject root;
    root["version"] = FILE_VERSION;
    root["objects"] = objects;
    return root;
}

QJsonObject Serializer::serializeObject(GeoObject* obj, int id, const std::unordered_map<GeoObject*, int>& idMap) const {
    /*const auto& items = m_adapter->geoGraphicsItems();
    std::unordered_map<GeoObject*, int> idMap;
    int i = 0;
    for (auto& [o, _] : items)
        idMap[o] = i++;*/

    auto ref = [&](GeoObject* o) -> int {
        auto it = idMap.find(o);
        return it != idMap.end() ? it->second : -1;
    };

    QJsonObject o;
    o["id"] = id;

    if (auto* ip = dynamic_cast<IntersectionPoint*>(obj)) {
        o["type"] = "IntersectionPoint";
        for (IntersectionSet* iset : m_adapter->intersectionSets()) {
            if (iset->first() == ip) {
                o["intersectionSet"] = ref(iset);
                o["index"] = 0;
                break;
            }
            if (iset->second() == ip) {
                o["intersectionSet"] = ref(iset);
                o["index"] = 1;
                break;
            }
        }
    } else if (auto* p = dynamic_cast<Point*>(obj)) {
        o["type"] = "Point";
        o["x"]    = p->x();
        o["y"]    = p->y();
    } else if (auto* l = dynamic_cast<Line*>(obj)) {
        o["type"] = "Line";
        o["p1"]   = ref(l->p1());
        o["p2"]   = ref(l->p2());
    } else if (auto* r = dynamic_cast<Ray*>(obj)) {
        o["type"] = "Ray";
        o["p1"]   = ref(r->p1());
        o["p2"]   = ref(r->p2());
    } else if (auto* s = dynamic_cast<Segment*>(obj)) {
        o["type"] = "Segment";
        o["p1"]   = ref(s->p1());
        o["p2"]   = ref(s->p2());
    } else if (auto* c = dynamic_cast<Circle*>(obj)) {
        o["type"]   = "Circle";
        o["center"] = ref(c->center());
        o["radius"] = ref(c->radiusPoint());
    } else if (auto* m = dynamic_cast<Midpoint*>(obj)) {
        o["type"] = "Midpoint";
        o["p1"]   = ref(m->p1());
        o["p2"]   = ref(m->p2());
    } else if (auto* p = dynamic_cast<Parallel*>(obj)) {
        o["type"]      = "Parallel";
        o["origin"]    = ref(p->origin());
        o["reference"] = ref(p->reference());
    } else if (auto* p = dynamic_cast<Perpendicular*>(obj)) {
        o["type"]      = "Perpendicular";
        o["origin"]    = ref(p->origin());
        o["reference"] = ref(p->reference());
    } else if (auto* f = dynamic_cast<PerpendicularFoot*>(obj)) {
        o["type"]  = "PerpendicularFoot";
        o["point"] = ref(f->point());
        o["line"]  = ref(f->line());
    } else if (auto* s = dynamic_cast<LineLineIntersection*>(obj)) {
        o["type"] = "LineLineIntersection";
        o["l1"]   = ref(s->L1());
        o["l2"]   = ref(s->L2());
    } else if (auto* s = dynamic_cast<LineCircleIntersection*>(obj)) {
        o["type"]   = "LineCircleIntersection";
        o["line"]   = ref(s->line());
        o["circle"] = ref(s->circle());
    } else if (auto* s = dynamic_cast<CircleCircleIntersection*>(obj)) {
        o["type"] = "CircleCircleIntersection";
        o["c1"]   = ref(s->c1());
        o["c2"]   = ref(s->c2());
    }

    return o;
}

// ── Load ─────────────────────────────────────────────────────────────────────

bool Serializer::load(const QString& filename) {
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

bool Serializer::deserializeScene(const QJsonArray& objects) {
    // Zwei Durchläufe:
    // 1. Alle Objekte erzeugen (damit Referenzen aufgelöst werden können)
    // 2. Referenzen auflösen und grafische Objekte anlegen

    std::unordered_map<int, GeoObject*> idMap;

    for (const auto& val : objects) {
        QJsonObject o    = val.toObject();
        int         id   = o["id"].toInt();
        QString     type = o["type"].toString();

        auto ref = [&](const QString& key) -> GeoObject* {
            int refId = o[key].toInt(-1);
            auto it = idMap.find(refId);
            return it != idMap.end() ? it->second : nullptr;
        };

        GeoObject* obj = nullptr;

        if (type == "IntersectionPoint") {
            int setId  = o["intersectionSet"].toInt(-1);
            int index  = o["index"].toInt(0);
            auto it = idMap.find(setId);
            if (it == idMap.end()) {
                m_lastError = "IntersectionSet nicht gefunden";
                return false;
            }
            auto* iset = dynamic_cast<IntersectionSet*>(it->second);
            if (!iset) { m_lastError = "Kein IntersectionSet"; return false; }
            obj = (index == 0) ? iset->first() : iset->second();
            // Kein addPoint nötig – Item wurde bereits von addIntersectionSet erzeugt
            idMap[id] = obj;
            continue; // kein erneutes addPoint
        } else if (type == "Point") {
            auto* p = m_scene->create<Point>(o["x"].toDouble(),
                                              o["y"].toDouble());
            m_adapter->addPoint(p);
            obj = p;

        } else if (type == "Line") {
            auto* p1 = dynamic_cast<Point*>(ref("p1"));
            auto* p2 = dynamic_cast<Point*>(ref("p2"));
            if (!p1 || !p2) { m_lastError = "Ungültige Referenz"; return false; }
            auto* l = m_scene->create<Line>(p1, p2);
            m_adapter->addLinearObject(l);
            obj = l;

        } else if (type == "Ray") {
            auto* p1 = dynamic_cast<Point*>(ref("p1"));
            auto* p2 = dynamic_cast<Point*>(ref("p2"));
            if (!p1 || !p2) { m_lastError = "Ungültige Referenz"; return false; }
            auto* r = m_scene->create<Ray>(p1, p2);
            m_adapter->addLinearObject(r);
            obj = r;

        } else if (type == "Segment") {
            auto* p1 = dynamic_cast<Point*>(ref("p1"));
            auto* p2 = dynamic_cast<Point*>(ref("p2"));
            if (!p1 || !p2) { m_lastError = "Ungültige Referenz"; return false; }
            auto* s = m_scene->create<Segment>(p1, p2);
            m_adapter->addLinearObject(s);
            obj = s;

        } else if (type == "Circle") {
            auto* center = dynamic_cast<Point*>(ref("center"));
            auto* radius = dynamic_cast<Point*>(ref("radius"));
            if (!center || !radius) { m_lastError = "Ungültige Referenz"; return false; }
            auto* c = m_scene->create<Circle>(center, radius);
            m_adapter->addCircle(c);
            obj = c;

        } else if (type == "Midpoint") {
            auto* p1 = dynamic_cast<Point*>(ref("p1"));
            auto* p2 = dynamic_cast<Point*>(ref("p2"));
            if (!p1 || !p2) { m_lastError = "Ungültige Referenz"; return false; }
            auto* m = m_scene->create<Midpoint>(p1, p2);
            m_adapter->addPoint(m);
            obj = m;

        } else if (type == "Parallel") {
            auto* origin = dynamic_cast<Point*>(ref("origin"));
            auto* refObj = dynamic_cast<LinearObject*>(ref("reference"));
            if (!origin || !refObj) { m_lastError = "Ungültige Referenz"; return false; }
            auto* p = m_scene->create<Parallel>(origin, refObj);
            m_adapter->addLinearObject(p->line());
            obj = p;

        } else if (type == "Perpendicular") {
            auto* origin = dynamic_cast<Point*>(ref("origin"));
            auto* refObj = dynamic_cast<LinearObject*>(ref("reference"));
            if (!origin || !refObj) { m_lastError = "Ungültige Referenz"; return false; }
            auto* p = m_scene->create<Perpendicular>(origin, refObj);
            m_adapter->addLinearObject(p->line());
            obj = p;

        } else if (type == "PerpendicularFoot") {
            auto* point = dynamic_cast<Point*>(ref("point"));
            auto* line  = dynamic_cast<LinearObject*>(ref("line"));
            if (!point || !line) { m_lastError = "Ungültige Referenz"; return false; }
            auto* f = m_scene->create<PerpendicularFoot>(point, line);
            m_adapter->addPoint(f);
            obj = f;

        } else if (type == "LineLineIntersection") {
            auto* l1 = dynamic_cast<LinearObject*>(ref("l1"));
            auto* l2 = dynamic_cast<LinearObject*>(ref("l2"));
            if (!l1 || !l2) { m_lastError = "Ungültige Referenz"; return false; }
            auto* s = m_scene->create<LineLineIntersection>(l1, l2);
            m_adapter->addIntersectionSet(s);
            obj = s;

        } else if (type == "LineCircleIntersection") {
            auto* line   = dynamic_cast<LinearObject*>(ref("line"));
            auto* circle = dynamic_cast<Circle*>(ref("circle"));
            if (!line || !circle) { m_lastError = "Ungültige Referenz"; return false; }
            auto* s = m_scene->create<LineCircleIntersection>(line, circle);
            m_adapter->addIntersectionSet(s);
            obj = s;

        } else if (type == "CircleCircleIntersection") {
            auto* c1 = dynamic_cast<Circle*>(ref("c1"));
            auto* c2 = dynamic_cast<Circle*>(ref("c2"));
            if (!c1 || !c2) { m_lastError = "Ungültige Referenz"; return false; }
            auto* s = m_scene->create<CircleCircleIntersection>(c1, c2);
            m_adapter->addIntersectionSet(s);
            obj = s;
        }

        if (obj) idMap[id] = obj;
    }
    return true;
}

// ── SVG Export ────────────────────────────────────────────────────────────────

bool Serializer::exportSVG(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = file.errorString();
        return false;
    }
    file.write(buildSVG().toUtf8());
    return true;
}

QString Serializer::buildSVG() const {
    // Bounding box aus allen Objekten bestimmen
    double minX =  1e10, minY =  1e10;
    double maxX = -1e10, maxY = -1e10;

    const auto& items = m_adapter->geoGraphicsItems();

    auto expand = [&](double x, double y) {
        minX = std::min(minX, x); minY = std::min(minY, y);
        maxX = std::max(maxX, x); maxY = std::max(maxY, y);
    };

    for (auto& [obj, item] : items) {
        if (!item->isVisible() || !obj->isValid()) continue;
        if (auto* p = dynamic_cast<Point*>(obj))
            expand(p->x(), p->y());
    }

    const double margin = 50.0;
    if (minX > maxX) { minX = -100; maxX = 100; minY = -100; maxY = 100; }
    minX -= margin; minY -= margin; maxX += margin; maxY += margin;

    double w = maxX - minX;
    double h = maxY - minY;

    // SVG-Koordinaten: Y-Achse invertieren (SVG hat Y nach unten)
    auto svgX = [&](double x) { return x - minX; };
    auto svgY = [&](double y) { return h - (y - minY); }; // Y-Flip

    const double ext = 10000.0; // Ausdehnung für Geraden

    QString svg;
    svg += QString(R"(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"
     width="%1" height="%2"
     viewBox="0 0 %1 %2">
<rect width="%1" height="%2" fill="white"/>
)").arg(w).arg(h);

    for (auto& [obj, item] : items) {
        if (!item->isVisible() || !obj->isValid()) continue;

        if (auto* lo = dynamic_cast<LinearObject*>(obj)) {
            double dx = lo->dx(), dy = lo->dy();
            double len = std::sqrt(dx*dx + dy*dy);
            if (len < 1e-10) continue;
            double ux = dx/len, uy = dy/len;

            double x1, y1, x2, y2;
            if (dynamic_cast<Segment*>(lo)) {
                x1 = lo->p1()->x(); y1 = lo->p1()->y();
                x2 = lo->p2()->x(); y2 = lo->p2()->y();
            } else if (dynamic_cast<Ray*>(lo)) {
                x1 = lo->p1()->x();       y1 = lo->p1()->y();
                x2 = lo->p1()->x()+ux*ext; y2 = lo->p1()->y()+uy*ext;
            } else {
                x1 = lo->p1()->x()-ux*ext; y1 = lo->p1()->y()-uy*ext;
                x2 = lo->p1()->x()+ux*ext; y2 = lo->p1()->y()+uy*ext;
            }

            svg += QString(R"(  <line x1="%1" y1="%2" x2="%3" y2="%4"
        stroke="black" stroke-width="1.5"/>
)")
                .arg(svgX(x1)).arg(svgY(y1))
                .arg(svgX(x2)).arg(svgY(y2));

        } else if (auto* c = dynamic_cast<Circle*>(obj)) {
            svg += QString(R"(  <circle cx="%1" cy="%2" r="%3"
        fill="none" stroke="black" stroke-width="1.5"/>
)")
                .arg(svgX(c->center()->x()))
                .arg(svgY(c->center()->y()))
                .arg(c->radius());

        } else if (auto* p = dynamic_cast<Point*>(obj)) {
            svg += QString(R"(  <circle cx="%1" cy="%2" r="4"
        fill="white" stroke="black" stroke-width="1.5"/>
)")
                .arg(svgX(p->x()))
                .arg(svgY(p->y()));
        }
    }

    svg += "</svg>\n";
    return svg;
}