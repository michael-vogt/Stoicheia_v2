#include "CopyCommand.h"

#include <ranges>

#include "Structs.h"
#include "constructions/CircleCircleIntersection.h"
#include "constructions/LineCircleIntersection.h"
#include "constructions/LineLineIntersection.h"
#include "constructions/Midpoint.h"
#include "constructions/Parallel.h"
#include "constructions/Perpendicular.h"
#include "constructions/PerpendicularFoot.h"
#include "geometry/Ray.h"
#include "geometry/Segment.h"

CopyCommand::CopyCommand(SceneAdapter *adapter, const std::unordered_set<GeoObject *> &selection, const QPointF &offset)
    : m_adapter(adapter), m_offset(offset)
{
    // Topologisch sortieren: Quellen vor Abhängigen
    std::unordered_set<GeoObject *> visited;
    std::function<void(GeoObject*)> collect = [&](GeoObject* obj) -> void {
        if (visited.contains(obj)) {
            return;
        }
        for (GeoObject* src : obj->sources()) {
            if (selection.contains(src)) {
                collect(src);
            }
        }
        if (!std::ranges::contains(m_originals, obj)) {
            m_originals.push_back(obj);
        }
    };

    for (GeoObject* obj : selection) {
        collect(obj);
    }
}

void CopyCommand::execute() {
    m_map.clear();
    m_copies.clear();
    createCopies();
}

void CopyCommand::undo() {
    removeCopies();
    m_map.clear();
    m_copies.clear();
}

auto CopyCommand::resolvePoint(Point *point) const -> Point* {
    auto iter = m_map.find(point);
    return iter != m_map.end() ? static_cast<Point*>(iter->second) : point;
}

auto CopyCommand::resolveLinearObject(LinearObject* linearObject) const -> LinearObject* {
    auto iter = m_map.find(linearObject);
    return iter != m_map.end() ? static_cast<LinearObject*>(iter->second) : linearObject;
}

auto CopyCommand::resolveCircle(Circle* circle) const -> Circle* {
    auto iter = m_map.find(circle);
    return iter != m_map.end() ? static_cast<Circle*>(iter->second) : circle;
}

void CopyCommand::createCopies() {
    for (GeoObject* orig : m_originals) {
        GeoObject* copy = nullptr;

        if (auto* point = dynamic_cast<Point*>(orig)) {
            auto* new_point = m_adapter->geoScene()->create<Point>(
                point->x() + m_offset.x(),
                point->y() + m_offset.y());
            m_adapter->addPoint(new_point);
            copy = new_point;
        } else if (auto* line = dynamic_cast<Line*>(orig)) {
            PointPairForLinearObject points{
                .point1=resolvePoint(line->p1()),
                .point2=resolvePoint(line->p2())};
            auto* new_line = m_adapter->geoScene()->create<Line>(points);
            m_adapter->addLinearObject(new_line);
            copy = new_line;
        } else if (auto* ray = dynamic_cast<Ray*>(orig)) {
            PointPairForLinearObject points{
                .point1=resolvePoint(line->p1()),
                .point2=resolvePoint(line->p2())};
            auto* new_ray = m_adapter->geoScene()->create<Ray>(points);
            m_adapter->addLinearObject(new_ray);
            copy = new_ray;
        } else if (auto* segment = dynamic_cast<Segment*>(orig)) {
            PointPairForLinearObject points{
                .point1=resolvePoint(line->p1()),
                .point2=resolvePoint(line->p2())};
            auto* new_segment = m_adapter->geoScene()->create<Segment>(points);
            m_adapter->addLinearObject(new_segment);
            copy = new_segment;
        } else if (auto* circle = dynamic_cast<Circle*>(orig)) {
            PointPairForCircle points{
                .center=resolvePoint(circle->center()),
                .radiusPoint=resolvePoint(circle->radiusPoint())
            };
            auto* new_circle = m_adapter->geoScene()->create<Circle>(points);
            m_adapter->addCircle(new_circle);
            copy = new_circle;
        } else if (auto* midpoint = dynamic_cast<Midpoint*>(orig)) {
            auto* new_midpoint = m_adapter->geoScene()->create<Midpoint>(
                resolvePoint(midpoint->point1()),
                resolvePoint(midpoint->point2()));
            m_adapter->addPoint(new_midpoint);
            copy = new_midpoint;
        } else if (auto* par = dynamic_cast<Parallel*>(orig)) {
            auto* new_par = m_adapter->geoScene()->create<Parallel>(
                resolvePoint(par->origin()),
                resolveLinearObject(par->reference()));
            m_adapter->addLinearObject(new_par->line());
            copy = new_par;
        } else if (auto* perp = dynamic_cast<Perpendicular*>(orig)) {
            auto* new_perp = m_adapter->geoScene()->create<Perpendicular>(
                resolvePoint(perp->origin()),
                resolveLinearObject(perp->reference()));
            m_adapter->addLinearObject(new_perp->line());
            copy = new_perp;
        } else if (auto* perp_foot = dynamic_cast<PerpendicularFoot*>(orig)) {
            auto* new_perp_foot = m_adapter->geoScene()->create<PerpendicularFoot>(
                resolvePoint(perp_foot->point()),
                resolveLinearObject(perp_foot->line()));
            m_adapter->addPoint(new_perp_foot);
            copy = new_perp_foot;
        } else if (auto* lli = dynamic_cast<LineLineIntersection*>(orig)) {
            auto* new_lli = m_adapter->geoScene()->create<LineLineIntersection>(
                resolveLinearObject(lli->L1()),
                resolveLinearObject(lli->L2()));
            m_adapter->addIntersectionSet(new_lli);
            copy = new_lli;
        } else if (auto* lci = dynamic_cast<LineCircleIntersection*>(orig)) {
            auto* new_lci = m_adapter->geoScene()->create<LineCircleIntersection>(
                resolveLinearObject(lci->line()),
                resolveCircle(lci->circle()));
            m_adapter->addIntersectionSet(new_lci);
            copy = new_lci;
        } else if (auto* cci = dynamic_cast<CircleCircleIntersection*>(orig)) {
            auto* new_cci = m_adapter->geoScene()->create<CircleCircleIntersection>(
                resolveCircle(cci->circle1()),
                resolveCircle(cci->circle2()));
            m_adapter->addIntersectionSet(new_cci);
            copy = new_cci;
        }

        if (copy != nullptr) {
            m_map[orig] = copy;
            m_copies.push_back(copy);
        }
    }
}

void CopyCommand::removeCopies() {
    // Rückwärts löschen - Abhängige vor Quellen
    for (auto & m_copie : std::views::reverse(m_copies)) {
        m_adapter->remove(m_copie);
    }
}
