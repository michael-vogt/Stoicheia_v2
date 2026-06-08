#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"


class CreatePointCommand : public Command {

    Q_OBJECT

public:
    CreatePointCommand(SceneAdapter* adapter, double pos_x, double pos_y);

    // Getter
    [[nodiscard]] auto point() const -> Point* { return m_point; }

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Punkt (%1, %2) erstellen").arg(m_x).arg(m_y); }

private:
    SceneAdapter* m_adapter;
    double m_x;
    double m_y;
    Point* m_point = nullptr;
};
