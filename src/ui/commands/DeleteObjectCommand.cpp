#include "DeleteObjectCommand.h"
#include "Structs.h"
#include "geometry/Ray.h"
#include "geometry/Segment.h"
#include <constructions/Midpoint.h>
#include <constructions/Parallel.h>
#include <constructions/Perpendicular.h>
#include <constructions/PerpendicularFoot.h>
#include <constructions/LineLineIntersection.h>
#include <constructions/LineCircleIntersection.h>
#include <constructions/CircleCircleIntersection.h>

DeleteObjectCommand::DeleteObjectCommand(SceneAdapter *adapter, GeoObject *object)
: m_adapter(adapter), m_object(object)
{}

void DeleteObjectCommand::execute() {
    m_undoFactory = buildUndoFactory();
    m_adapter->remove(m_object);
}

void DeleteObjectCommand::undo() {
    if (m_undoFactory) {
        m_undoFactory();
    }
}

auto DeleteObjectCommand::buildUndoFactory() -> std::function<void()> {
    // Zustand vor dem Löschen erfassen - je nach Typ
    if (auto* point = dynamic_cast<Point*>(m_object)) {
        double coord_x = point->x();
        double coord_y = point->y();
        return [this, coord_x, coord_y]() -> void {
            GeoObject* oldObj = m_object;
            auto* point = m_adapter->geoScene()->create<Point>(coord_x, coord_y);
            m_adapter->addPoint(point);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=point});
            m_object = point;
        };
    }

    if (auto* line = dynamic_cast<Line*>(m_object)) {
        Point* point1 = line->point1();
        Point* point2 = line->point2();
        return [this, point1, point2]() -> void {
            GeoObject* oldObj = m_object;
            auto* line = m_adapter->geoScene()->create<Line>(PointPairForLinearObject{.point1=point1, .point2=point2});
            m_adapter->addLinearObject(line);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=line});
            m_object = line;
        };
    }

    if (auto* ray = dynamic_cast<Ray*>(m_object)) {
        Point* point1 = ray->point1();
        Point* point2 = ray->point2();
        return [this, point1, point2]() -> void {
            GeoObject* oldObj = m_object;
            auto* ray = m_adapter->geoScene()->create<Ray>(PointPairForLinearObject{.point1=point1, .point2=point2});
            m_adapter->addLinearObject(ray);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=ray});
            m_object = ray;
        };
    }

    if (auto* segment = dynamic_cast<Segment*>(m_object)) {
        Point* point1 = segment->point1();
        Point* point2 = segment->point2();
        return [this, point1, point2]() -> void {
            GeoObject* oldObj = m_object;
            auto* segment = m_adapter->geoScene()->create<Segment>(PointPairForLinearObject{.point1=point1, .point2=point2});
            m_adapter->addLinearObject(segment);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=segment});
            m_object = segment;
        };
    }

    if (auto* circle = dynamic_cast<Circle*>(m_object)) {
        Point* center = circle->center();
        Point* radiusPoint = circle->radiusPoint();
        return [this, center, radiusPoint]() -> void {
            GeoObject* oldObj = m_object;
            auto* circle = m_adapter->geoScene()->create<Circle>(PointPairForCircle{.center=center,.radiusPoint=radiusPoint});
            m_adapter->addCircle(circle);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=circle});
            m_object = circle;
        };
    }

    if (auto* midpoint = dynamic_cast<Midpoint*>(m_object)) {
        Point* point1 = midpoint->point1();
        Point* point2 = midpoint->point2();
        return [this, point1, point2]() -> void {
            GeoObject* oldObj = m_object;
            auto* mid = m_adapter->geoScene()->create<Midpoint>(point1, point2);
            m_adapter->addPoint(mid);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=mid});
            m_object = mid;
        };
    }

    if (auto* parallel = dynamic_cast<Parallel*>(m_object)) {
        Point* origin = parallel->origin();
        LinearObject* ref = parallel->reference();
        return [this, origin, ref]() -> void {
            GeoObject* oldObj = m_object;
            auto* par = m_adapter->geoScene()->create<Parallel>(origin, ref);
            m_adapter->addLinearObject(par->line());
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=par});
            m_object = par;
        };
    }

    if (auto* perp = dynamic_cast<Perpendicular*>(m_object)) {
        Point* origin = perp->origin();
        LinearObject* ref = perp->reference();
        return [this, origin, ref]() -> void {
            GeoObject* oldObj = m_object;
            auto* perp = m_adapter->geoScene()->create<Perpendicular>(origin, ref);
            m_adapter->addLinearObject(perp->line());
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=perp});
            m_object = perp;
        };
    }

    if (auto* perp_foot = dynamic_cast<PerpendicularFoot*>(m_object)) {
        Point* point = perp_foot->point();
        LinearObject* line = perp_foot->line();
        return [this, point, line]() -> void {
            GeoObject* oldObj = m_object;
            auto* foot = m_adapter->geoScene()->create<PerpendicularFoot>(point, line);
            m_adapter->addPoint(foot);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=foot});
            m_object = foot;
        };
    }

    // Schnittpunkte - IntersectionSet
    if (auto* lli = dynamic_cast<LineLineIntersection*>(m_object)) {
        LinearObject* lo1 = lli->line1();
        LinearObject* lo2 = lli->line2();
        return [this, lo1, lo2]() -> void {
            GeoObject* oldObj = m_object;
            auto* intersectionSet = m_adapter->geoScene()->create<LineLineIntersection>(lo1, lo2);
            m_adapter->addIntersectionSet(intersectionSet);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=intersectionSet});
            m_object = intersectionSet;
        };
    }

    if (auto* lci = dynamic_cast<LineCircleIntersection*>(m_object)) {
        LinearObject* linearObject = lci->line();
        Circle* circle = lci->circle();
        return [this, linearObject, circle]() -> void {
            GeoObject* oldObj = m_object;
            auto* intersectionSet = m_adapter->geoScene()->create<LineCircleIntersection>(linearObject, circle);
            m_adapter->addIntersectionSet(intersectionSet);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=intersectionSet});
            m_object = intersectionSet;
        };
    }

    if (auto* cci = dynamic_cast<CircleCircleIntersection*>(m_object)) {
        Circle* circle1 = cci->circle1();
        Circle* circle2 = cci->circle2();
        return [this, circle1, circle2]() -> void {
            GeoObject* oldObj = m_object;
            auto* intersectionSet = m_adapter->geoScene()->create<CircleCircleIntersection>(circle1, circle2);
            m_adapter->addIntersectionSet(intersectionSet);
            m_adapter->transferStyle({.oldGeoObject=oldObj, .newGeoObject=intersectionSet});
            m_object = intersectionSet;
        };
    }

    return {}; // unbekannter Typ
}