#pragma once
#include "Command.h"
#include "constructions/Perpendicular.h"
#include "ui/SceneAdapter.h"

class CreatePerpendicularCommand : public Command {

    Q_OBJECT

public:
    CreatePerpendicularCommand(SceneAdapter* adapter, Point* origin, LinearObject* reference);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Senkrechte erstellen"); }

private:
    SceneAdapter* m_adapter;
    Point* m_origin;
    LinearObject* m_reference;
    Perpendicular* m_result = nullptr;
};