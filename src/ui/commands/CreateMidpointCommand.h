#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class CreateMidpointCommand : public Command {

    Q_OBJECT

public:
    CreateMidpointCommand(SceneAdapter* adapter, Point* p1, Point* p2);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Mittelpunkt erstellen"); }

private:
    SceneAdapter* m_adapter;
    Point* m_p1;
    Point* m_p2;
    Point* m_result = nullptr;
};
