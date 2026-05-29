#include "DeleteObjectCommand.h"
#include <constructions/Midpoint.h>
#include <constructions/Parallel.h>
#include <constructions/Perpendicular.h>
#include <constructions/PerpendicularFoot.h>
#include <constructions/LineLineIntersection.h>
#include <constructions/LineCircleIntersection.h>
#include <constructions/CircleCircleIntersection.h>

DeleteObjectCommand::DeleteObjectCommand(SceneAdapter *adapter, GeoObject *object) : m_adapter(adapter), m_object(object)
{}

std::function<void()> DeleteObjectCommand::buildUndoFactory() {
    // Zustand vor dem Löschen erfassen - je nach Typ
    if (auto* p = dynamic_cast<Point*>(m_object)) {
        double x = p->x(), y = p->y();
        return [this, x, y]() {
            auto* pt = m_adapter->geoScene()->create<Point>(x, y);
            m_adapter->addPoint(pt);
            m_object = pt;
        };
    }

    if (auto* l = dynamic_cast<Line*>(m_object)) {
        Point* p1 = l->p1();
        Point* p2 = l->p2();
        return [this, p1, p2]() {
            auto* line = m_adapter->geoScene()->create<Line>(p1, p2);
            m_adapter->addLinearObject(line);
            m_object = line;
        };
    }

    if (auto* r = dynamic_cast<Ray*>(m_object)) {
        Point* p1 = r->p1();
        Point* p2 = r->p2();
        return [this, p1, p2]() {
            auto* ray = m_adapter->geoScene()->create<Ray>(p1, p2);
            m_adapter->addLinearObject(ray);
            m_object = ray;
        };
    }

    if (auto* s = dynamic_cast<Segment*>(m_object)) {
        Point* p1 = s->p1();
        Point* p2 = s->p2();

        Point* p1Copy = nullptr;
        Point* p2Copy = nullptr;
        if (p1)
            p1Copy = new Point(p1->x(), p1->y());
        if (p2)
            p2Copy = new Point(p2->x(), p2->y());

        return [this, p1Copy, p2Copy]() {
            auto* segment = m_adapter->geoScene()->create<Segment>(p1Copy, p2Copy);
            m_adapter->addLinearObject(segment);
            m_object = segment;
        };
    }

    if (auto* c = dynamic_cast<Circle*>(m_object)) {
        Point* center = c->center();
        Point* rp = c->radiusPoint();
        return [this, center, rp]() {
            auto* circle = m_adapter->geoScene()->create<Circle>(center, rp);
            m_adapter->addCircle(circle);
            m_object = circle;
        };
    }

    if (auto* m = dynamic_cast<Midpoint*>(m_object)) {
        Point* p1 = m->p1();
        Point* p2 = m->p2();
        return [this, p1, p2]() {
            auto* mid = m_adapter->geoScene()->create<Midpoint>(p1, p2);
            m_adapter->addPoint(mid);
            m_object = mid;
        };
    }

    if (auto* p = dynamic_cast<Parallel*>(m_object)) {
        Point* origin = p->origin();
        LinearObject* ref = p->reference();
        return [this, origin, ref]() {
            auto* par = m_adapter->geoScene()->create<Parallel>(origin, ref);
            m_adapter->addLinearObject(par->line());
            m_object = par;
        };
    }

    if (auto* p = dynamic_cast<Perpendicular*>(m_object)) {
        Point* origin = p->origin();
        LinearObject* ref = p->reference();
        return [this, origin, ref]() {
            auto* perp = m_adapter->geoScene()->create<Perpendicular>(origin, ref);
            m_adapter->addLinearObject(perp->line());
            m_object = perp;
        };
    }

    if (auto* f = dynamic_cast<PerpendicularFoot*>(m_object)) {
        Point* pt = f->point();
        LinearObject* line = f->line();
        return [this, pt, line]() {
            auto* foot = m_adapter->geoScene()->create<PerpendicularFoot>(pt, line);
            m_adapter->addPoint(foot);
            m_object = foot;
        };
    }

    // Schnittpunkte - IntersectionSet
    if (auto* ll = dynamic_cast<LineLineIntersection*>(m_object)) {
        LinearObject* l1 = ll->L1();
        LinearObject* l2 = ll->L2();
        return [this, l1, l2]() {
            auto* s = m_adapter->geoScene()->create<LineLineIntersection>(l1, l2);
            m_adapter->addIntersectionSet(s);
            m_object = s;
        };
    }

    if (auto* lc = dynamic_cast<LineCircleIntersection*>(m_object)) {
        LinearObject* l = lc->line();
        Circle* c = lc->circle();
        return [this, l, c]() {
            auto* s = m_adapter->geoScene()->create<LineCircleIntersection>(l, c);
            m_adapter->addIntersectionSet(s);
            m_object = s;
        };
    }

    if (auto* cc = dynamic_cast<CircleCircleIntersection*>(m_object)) {
        Circle* c1 = cc->c1();
        Circle* c2 = cc->c2();
        return [this, c1, c2]() {
            auto* s = m_adapter->geoScene()->create<CircleCircleIntersection>(c1, c2);
            m_adapter->addIntersectionSet(s);
            m_object = s;
        };
    }

    return {}; // unbekannter Typ
}

void DeleteObjectCommand::execute() {
    m_undoFactory = buildUndoFactory();
    m_object->detach();
    m_adapter->removeGraphicsOnly(m_object);
}

void DeleteObjectCommand::undo() {
    if (m_undoFactory) {
        m_undoFactory();
    }
}

QString DeleteObjectCommand::description() const {
    return QObject::tr("Objekt löschen");
}
