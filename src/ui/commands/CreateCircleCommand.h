#pragma once
#include "Command.h"
#include "CreatePointCommand.h"
#include "Structs.h"
#include "ui/SceneAdapter.h"

class CreateCircleCommand : public Command {

    Q_OBJECT

public:
    explicit CreateCircleCommand(SceneAdapter* adapter, PointPairForCircle points);
    explicit CreateCircleCommand(SceneAdapter* adapter, CreatePointCommandPairForCircle cmds, PointPairForCircle points);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Kreis erstellen"); }

    [[nodiscard]] auto circle() const -> Circle* { return m_circle; }

private:
    [[nodiscard]] auto resolveP1() const -> Point* { return (m_p1Cmd != nullptr) ? m_p1Cmd->point() : m_center; }
    [[nodiscard]] auto resolveP2() const -> Point* { return (m_p2Cmd != nullptr) ? m_p2Cmd->point() : m_radiusPoint; }

    SceneAdapter* m_adapter;
    CreatePointCommand* m_p1Cmd = nullptr;
    CreatePointCommand* m_p2Cmd = nullptr;
    Point* m_center;
    Point* m_radiusPoint;
    Circle* m_circle = nullptr;
};
