#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class CreateLineCommand : public Command {
    SceneAdapter* m_adapter;
    Point* m_p1;
    Point* m_p2;
    Line* m_line = nullptr;

public:
    CreateLineCommand(SceneAdapter* adapter, Point* p1, Point* p2);

    void execute() override;
    void undo() override;

    QString description() const override;

    Line* line() const { return m_line; }
};
