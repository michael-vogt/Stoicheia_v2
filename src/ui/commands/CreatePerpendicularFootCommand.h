#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class CreatePerpendicularFootCommand : public Command {

    Q_OBJECT

public:
    CreatePerpendicularFootCommand(SceneAdapter* adapter, Point* point, LinearObject* line);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Lotfußpunkt erstellen"); }

private:
    SceneAdapter* m_adapter;
    Point* m_point;
    LinearObject* m_line;
    GeoObject* m_result = nullptr;
};