#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class CreateParallelCommand : public Command {
    SceneAdapter* m_adapter;
    Point* m_origin;
    LinearObject* m_reference;
    GeoObject* m_result = nullptr;

public:
    CreateParallelCommand(SceneAdapter* adapter, Point* origin, LinearObject* reference);

    void execute() override;
    void undo() override;
    QString description() const override;
};
