#include "ConstrainPointCommand.h"
#include "constructions/ConstrainedPoint.h"

// ─── ConstrainPointToLineCommand ──────────────────────────────────────────────

ConstrainPointToLineCommand::ConstrainPointToLineCommand(
    SceneAdapter* adapter, Point* point, LinearObject* line)
    : m_adapter(adapter), m_freePoint(point), m_line(line)
    , m_oldX(point->x()), m_oldY(point->y())
{}

void ConstrainPointToLineCommand::execute() {
    m_rewired.clear();

    // Eingeschränkten Punkt an der projizierten Position erzeugen
    m_constrained = m_adapter->geoScene()->create<ConstrainedPointOnLine>(
        m_line, DoublePair{m_oldX, m_oldY});
    m_adapter->addPoint(m_constrained);

    // Alle Abhängigen des alten Punkts auf den neuen umhängen
    std::vector<GeoObject*> deps(
        m_freePoint->dependents().begin(), m_freePoint->dependents().end());
    for (GeoObject* dep : deps) {
        m_freePoint->removeDependent(dep);
        m_constrained->addDependent(dep);
        dep->replaceSource(GeoObjectPair{m_freePoint, m_constrained});
        dep->recompute();
        m_rewired.push_back(dep);
    }

    // Alten Punkt aus der grafischen Szene entfernen (Geometrie bleibt für Undo)
    m_adapter->removeGraphicsOnly(m_freePoint);
}

void ConstrainPointToLineCommand::undo() {
    // Abhängige zurückhängen
    for (GeoObject* dep : m_rewired) {
        if (!m_constrained->dependents().contains(dep)) continue;
        m_constrained->removeDependent(dep);
        m_freePoint->addDependent(dep);
        dep->replaceSource(GeoObjectPair{m_constrained, m_freePoint});
        dep->recompute();
    }
    m_rewired.clear();

    m_freePoint->moveTo(m_oldX, m_oldY);
    m_adapter->addPoint(m_freePoint);
    m_adapter->remove(m_constrained);
    m_constrained = nullptr;
}

// ─── ConstrainPointToCircleCommand ───────────────────────────────────────────

ConstrainPointToCircleCommand::ConstrainPointToCircleCommand(
    SceneAdapter* adapter, Point* point, Circle* circle)
    : m_adapter(adapter), m_freePoint(point), m_circle(circle)
    , m_oldX(point->x()), m_oldY(point->y())
{}

void ConstrainPointToCircleCommand::execute() {
    m_rewired.clear();

    m_constrained = m_adapter->geoScene()->create<ConstrainedPointOnCircle>(
        m_circle, DoublePair{m_oldX, m_oldY});
    m_adapter->addPoint(m_constrained);

    std::vector<GeoObject*> deps(
        m_freePoint->dependents().begin(), m_freePoint->dependents().end());
    for (GeoObject* dep : deps) {
        m_freePoint->removeDependent(dep);
        m_constrained->addDependent(dep);
        dep->replaceSource(GeoObjectPair{m_freePoint, m_constrained});
        dep->recompute();
        m_rewired.push_back(dep);
    }

    m_adapter->removeGraphicsOnly(m_freePoint);
}

void ConstrainPointToCircleCommand::undo() {
    for (GeoObject* dep : m_rewired) {
        if (!m_constrained->dependents().contains(dep)) continue;
        m_constrained->removeDependent(dep);
        m_freePoint->addDependent(dep);
        dep->replaceSource(GeoObjectPair{m_constrained, m_freePoint});
        dep->recompute();
    }
    m_rewired.clear();

    m_freePoint->moveTo(m_oldX, m_oldY);
    m_adapter->addPoint(m_freePoint);
    m_adapter->remove(m_constrained);
    m_constrained = nullptr;
}