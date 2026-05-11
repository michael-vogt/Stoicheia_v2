#pragma once
#include "Command.h"
#include "CreatePointCommand.h"
#include "LinearObjectType.h"
#include "ui/SceneAdapter.h"

class CreateCircleCommand : public Command {
    SceneAdapter* m_adapter;
    CreatePointCommand* m_p1Cmd = nullptr;
    CreatePointCommand* m_p2Cmd = nullptr;
    Point* m_p1;
    Point* m_p2;
    Circle* m_circle = nullptr;

    Point* resolveP1() const;
    Point* resolveP2() const;

public:
    explicit CreateCircleCommand(SceneAdapter* adapter, Point* p1, Point* p2);
    explicit CreateCircleCommand(SceneAdapter* adapter, CreatePointCommand* p1Cmd, CreatePointCommand* p2Cmd, Point* p1, Point* p2);

    void execute() override;
    void undo() override;

    [[nodiscard]] QString description() const override;

    [[nodiscard]] Circle* circle() const { return m_circle; }
};
