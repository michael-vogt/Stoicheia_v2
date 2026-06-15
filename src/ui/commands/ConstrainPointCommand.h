#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"
#include "geometry/GeoObject.h"
#include "geometry/LinearObject.h"
#include "geometry/Circle.h"

// Ersetzt einen freien Punkt durch einen auf einem LinearObject eingeschränkten Punkt.
class ConstrainPointToLineCommand : public Command {
    Q_OBJECT
public:
    ConstrainPointToLineCommand(SceneAdapter* adapter, Point* point, LinearObject* line);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Punkt auf Gerade einschränken"); }

private:
    SceneAdapter*  m_adapter;
    Point*         m_freePoint;   // ursprünglicher freier Punkt (bleibt in Scene für Undo)
    LinearObject*  m_line;
    Point*         m_constrained = nullptr; // neu erzeugter eingeschränkter Punkt
    double         m_oldX, m_oldY;
    std::vector<GeoObject*> m_rewired;
};

// Ersetzt einen freien Punkt durch einen auf einem Kreis eingeschränkten Punkt.
class ConstrainPointToCircleCommand : public Command {
    Q_OBJECT
public:
    ConstrainPointToCircleCommand(SceneAdapter* adapter, Point* point, Circle* circle);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Punkt auf Kreis einschränken"); }

private:
    SceneAdapter*  m_adapter;
    Point*         m_freePoint;
    Circle*        m_circle;
    Point*         m_constrained = nullptr;
    double         m_oldX, m_oldY;
    std::vector<GeoObject*> m_rewired;
};