#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"


class CreatePointCommand : public Command {

    Q_OBJECT

public:
    CreatePointCommand(SceneAdapter* adapter, double x, double y);

    // Getter
    Point* point() const { return m_point; }

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Punkt (%1, %2) erstellen").arg(m_x).arg(m_y); }

private:
    SceneAdapter* m_adapter;
    double m_x;
    double m_y;
    Point* m_point = nullptr;
};
