#include "CopyCommand.h"

#include "constructions/CircleCircleIntersection.h"
#include "constructions/LineCircleIntersection.h"
#include "constructions/LineLineIntersection.h"
#include "constructions/Midpoint.h"
#include "constructions/Parallel.h"
#include "constructions/Perpendicular.h"
#include "constructions/PerpendicularFoot.h"

CopyCommand::CopyCommand(SceneAdapter *adapter, const std::unordered_set<GeoObject *> &selection, const QPointF &offset)
    : m_adapter(adapter), m_offset(offset)
{
    // Topologisch sortieren: Quellen vor Abhängigen
    std::unordered_set<GeoObject *> visited;
    std::function<void(GeoObject*)> collect = [&](GeoObject* obj) {
        if (visited.contains(obj)) return;
        for (GeoObject* src : obj->sources())
            if (selection.contains(src))
                collect(src);
        if (!std::ranges::contains(m_originals, obj))
            m_originals.push_back(obj);
    };

    for (GeoObject* obj : selection)
        collect(obj);
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

Point* CopyCommand::resolvePoint(Point *p) const {
    auto it = m_map.find(p);
    return it != m_map.end() ? static_cast<Point*>(it->second) : p;
}

LinearObject* CopyCommand::resolveLinearObject(LinearObject* lo) const {
    auto it = m_map.find(lo);
    return it != m_map.end() ? static_cast<LinearObject*>(it->second) : lo;
}

Circle* CopyCommand::resolveCircle(Circle* c) const {
    auto it = m_map.find(c);
    return it != m_map.end() ? static_cast<Circle*>(it->second) : c;
}

void CopyCommand::createCopies() {
    for (GeoObject* orig : m_originals) {
        GeoObject* copy = nullptr;

        if (auto* p = dynamic_cast<Point*>(orig)) {
            auto* np = m_adapter->geoScene()->create<Point>(
                p->x() + m_offset.x(),
                p->y() + m_offset.y());
            m_adapter->addPoint(np);
            copy = np;
        } else if (auto* l = dynamic_cast<Line*>(orig)) {
            auto* nl = m_adapter->geoScene()->create<Line>(
                resolvePoint(l->p1()),
                resolvePoint(l->p2()));
            m_adapter->addLinearObject(nl);
            copy = nl;
        } else if (auto* r = dynamic_cast<Ray*>(orig)) {
            auto* nr = m_adapter->geoScene()->create<Ray>(
                resolvePoint(r->p1()),
                resolvePoint(r->p2()));
            m_adapter->addLinearObject(nr);
            copy = nr;
        } else if (auto* s = dynamic_cast<Segment*>(orig)) {
            auto* ns = m_adapter->geoScene()->create<Segment>(
                resolvePoint(s->p1()),
                resolvePoint(s->p2()));
            m_adapter->addLinearObject(ns);
            copy = ns;
        } else if (auto* c = dynamic_cast<Circle*>(orig)) {
            auto* nc = m_adapter->geoScene()->create<Circle>(
                resolvePoint(c->center()),
                resolvePoint(c->radiusPoint()));
            m_adapter->addCircle(nc);
            copy = nc;
        } else if (auto* m = dynamic_cast<Midpoint*>(orig)) {
            auto* nm = m_adapter->geoScene()->create<Midpoint>(
                resolvePoint(m->p1()),
                resolvePoint(m->p2()));
            m_adapter->addPoint(nm);
            copy = nm;
        } else if (auto* par = dynamic_cast<Parallel*>(orig)) {
            auto* np = m_adapter->geoScene()->create<Parallel>(
                resolvePoint(par->origin()),
                resolveLinearObject(par->reference()));
            m_adapter->addLinearObject(np->line());
            copy = np;
        } else if (auto* perp = dynamic_cast<Perpendicular*>(orig)) {
            auto* np = m_adapter->geoScene()->create<Perpendicular>(
                resolvePoint(perp->origin()),
                resolveLinearObject(perp->reference()));
            m_adapter->addLinearObject(np->line());
            copy = np;
        } else if (auto* f = dynamic_cast<PerpendicularFoot*>(orig)) {
            auto* nf = m_adapter->geoScene()->create<PerpendicularFoot>(
                resolvePoint(f->point()),
                resolveLinearObject(f->line()));
            m_adapter->addPoint(nf);
            copy = nf;
        } else if (auto* ll = dynamic_cast<LineLineIntersection*>(orig)) {
            auto* ni = m_adapter->geoScene()->create<LineLineIntersection>(
                resolveLinearObject(ll->L1()),
                resolveLinearObject(ll->L2()));
            m_adapter->addIntersectionSet(ni);
            copy = ni;
        } else if (auto* lc = dynamic_cast<LineCircleIntersection*>(orig)) {
            auto* ni = m_adapter->geoScene()->create<LineCircleIntersection>(
                resolveLinearObject(lc->line()),
                resolveCircle(lc->circle()));
            m_adapter->addIntersectionSet(ni);
            copy = ni;
        } else if (auto* cc = dynamic_cast<CircleCircleIntersection*>(orig)) {
            auto* ni = m_adapter->geoScene()->create<CircleCircleIntersection>(
                resolveCircle(cc->c1()),
                resolveCircle(cc->c2()));
            m_adapter->addIntersectionSet(ni);
            copy = ni;
        }

        if (copy) {
            m_map[orig] = copy;
            m_copies.push_back(copy);
        }
    }
}

void CopyCommand::removeCopies() {
    // Rückwärts löschen - Abhängige vor Quellen
    for (auto it = m_copies.rbegin(); it != m_copies.rend(); ++it)
        m_adapter->remove(*it);
}
