#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class IntersectionSet;

// Erzeugt den Schnittpunkt zweier Objekte.
// Unterstützt: Line∩Line, Line∩Circle, Circle∩Circle
class CreateIntersectionCommand : public Command {
    SceneAdapter* m_adapter;
    GeoObject* m_obj1;
    GeoObject* m_obj2;
    IntersectionSet* m_result = nullptr;

public:
    CreateIntersectionCommand(SceneAdapter* adapter, GeoObject* obj1, GeoObject* obj2);

    void execute() override;
    void undo() override;
    QString description() const override;
};