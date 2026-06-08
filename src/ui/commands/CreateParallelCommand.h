#pragma once
#include "Command.h"
#include "constructions/Parallel.h"
#include "ui/SceneAdapter.h"

class CreateParallelCommand : public Command {

    Q_OBJECT

public:
    CreateParallelCommand(SceneAdapter* adapter, Point* origin, LinearObject* reference);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Parallele erstellen"); }

private:
    SceneAdapter* m_adapter;
    Point* m_origin;
    LinearObject* m_reference;
    Parallel* m_result = nullptr;
};
