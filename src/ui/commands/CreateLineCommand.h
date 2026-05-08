#pragma once
#include "Command.h"
#include "LinearObjectType.h"
#include "ui/SceneAdapter.h"

class CreateLineCommand : public Command {
    SceneAdapter* m_adapter;
    Point* m_p1;
    Point* m_p2;
    LinearObject* m_linear = nullptr;
    LinearObjectType m_type;

public:
    CreateLineCommand(SceneAdapter* adapter, Point* p1, Point* p2, LinearObjectType type);

    void execute() override;
    void undo() override;

    QString description() const override;

    LinearObject* linear() const { return m_linear; }
};
