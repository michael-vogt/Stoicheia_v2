#pragma once
#include "Command.h"
#include "CreatePointCommand.h"
#include "LinearObjectType.h"
#include "ui/SceneAdapter.h"

class CreateLineCommand : public Command {
    Q_OBJECT
    SceneAdapter* m_adapter;
    CreatePointCommand* m_p1Cmd = nullptr;
    CreatePointCommand* m_p2Cmd = nullptr;
    Point* m_p1;
    Point* m_p2;
    LinearObjectType m_type;
    LinearObject* m_linear = nullptr;

    Point* resolveP1() const;
    Point* resolveP2() const;

public:
    CreateLineCommand(SceneAdapter* adapter, Point* p1, Point* p2, LinearObjectType type);
    CreateLineCommand(SceneAdapter* adapter, CreatePointCommand* p1Cmd, CreatePointCommand* p2Cmd, Point* p1, Point* p2, LinearObjectType type);

    void execute() override;
    void undo() override;

    [[nodiscard]] QString description() const override;

    [[nodiscard]] LinearObject* linear() const { return m_linear; }
};
