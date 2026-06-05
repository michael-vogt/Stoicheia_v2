#pragma once
#include "Command.h"
#include "CreatePointCommand.h"
#include "LinearObjectType.h"
#include "ui/SceneAdapter.h"

class CreateCircleCommand : public Command {

    Q_OBJECT

public:
    explicit CreateCircleCommand(SceneAdapter* adapter, Point* p1, Point* p2);
    explicit CreateCircleCommand(SceneAdapter* adapter, CreatePointCommand* p1Cmd, CreatePointCommand* p2Cmd, Point* p1, Point* p2);

    void execute() override;
    void undo() override;
    [[nodiscard]] QString description() const override { return tr("Kreis erstellen"); }

    [[nodiscard]] Circle* circle() const { return m_circle; }

private:
    Point* resolveP1() const { return m_p1Cmd ? m_p1Cmd->point() : m_p1; }
    Point* resolveP2() const { return m_p2Cmd ? m_p2Cmd->point() : m_p2; }

    SceneAdapter* m_adapter;
    CreatePointCommand* m_p1Cmd = nullptr;
    CreatePointCommand* m_p2Cmd = nullptr;
    Point* m_p1;
    Point* m_p2;
    Circle* m_circle = nullptr;
};
