#pragma once
#include "Command.h"
#include "CreatePointCommand.h"
#include "LinearObjectType.h"
#include "Structs.h"
#include "ui/SceneAdapter.h"

class CreateLineCommand : public Command {

    Q_OBJECT

public:
    CreateLineCommand(SceneAdapter* adapter, PointPairForLinearObject points, LinearObjectType type);
    CreateLineCommand(SceneAdapter* adapter, CreatePointCommandPairForLinearObject cmds, PointPairForLinearObject points, LinearObjectType type);

    // Getter
    [[nodiscard]] auto linear() const -> LinearObject* { return m_linear; }

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override;

private:
    [[nodiscard]] auto resolvePoint1() const -> Point* { return (m_p1Cmd != nullptr) ? m_p1Cmd->point() : m_point1; }
    [[nodiscard]] auto resolvePoint2() const -> Point* { return (m_p2Cmd != nullptr) ? m_p2Cmd->point() : m_point2; }

    SceneAdapter* m_adapter;
    CreatePointCommand* m_p1Cmd = nullptr;
    CreatePointCommand* m_p2Cmd = nullptr;
    Point* m_point1;
    Point* m_point2;
    LinearObjectType m_type;
    LinearObject* m_linear = nullptr;
};
