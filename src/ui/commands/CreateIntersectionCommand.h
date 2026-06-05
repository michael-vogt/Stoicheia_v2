#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class IntersectionSet;


class CreateIntersectionCommand : public Command {

    Q_OBJECT

public:
    CreateIntersectionCommand(SceneAdapter* adapter, GeoObject* obj1, GeoObject* obj2);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Schnittpunkt erstellen"); }

private:
    SceneAdapter* m_adapter;
    GeoObject* m_obj1;
    GeoObject* m_obj2;
    IntersectionSet* m_result = nullptr;
};