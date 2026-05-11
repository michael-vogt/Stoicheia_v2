#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"


class CreatePointCommand : public Command {
    SceneAdapter* m_adapter;
    double m_x;
    double m_y;
    Point* m_point = nullptr;

public:
    CreatePointCommand(SceneAdapter* adapter, double x, double y);

    void execute() override;
    void undo() override;
    QString description() const override;

    Point* point() const { return m_point; }
};
