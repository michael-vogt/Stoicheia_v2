#pragma once
#include "Command.h"
#include "constructions/Perpendicular.h"
#include "ui/SceneAdapter.h"

class CreatePerpendicularCommand : public Command {
    SceneAdapter* m_adapter;
    Point* m_origin;
    LinearObject* m_reference;
    Perpendicular* m_result = nullptr;

public:
    CreatePerpendicularCommand(SceneAdapter* adapter, Point* origin, LinearObject* reference);

    void execute() override;
    void undo() override;
    QString description() const override;
};