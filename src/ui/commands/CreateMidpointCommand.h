#pragma once
#include "Command.h"
#include "Structs.h"
#include "ui/SceneAdapter.h"

class CreateMidpointCommand : public Command {

    Q_OBJECT

public:
    CreateMidpointCommand(SceneAdapter* adapter, PointPairForLinearObject points);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Mittelpunkt erstellen"); }

private:
    SceneAdapter* m_adapter;
    Point* m_point1;
    Point* m_point2;
    Point* m_result = nullptr;
};
