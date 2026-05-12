#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class CreatePerpendicularFootCommand : public Command {
    SceneAdapter* m_adapter;
    Point* m_point;
    LinearObject* m_line;
    GeoObject* m_result = nullptr;

public:
    CreatePerpendicularFootCommand(SceneAdapter* adapter, Point* point, LinearObject* line);

    void execute() override;
    void undo() override;
    QString description() const override;
};