#pragma once
#include "Command.h"
#include "MoveCommand.h"
#include "geometry/Point.h"

class MovePointCommand : public MoveCommand {

    Q_OBJECT

public:
    MovePointCommand(Point* point, double new_x, double new_y);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Punkt verschieben nach (%1, %2)").arg(m_newX).arg(m_newY); }

    void setTarget(double new_x, double new_y) override;

private:
    Point* m_point;
    double m_oldX, m_oldY;
    double m_newX, m_newY;
};
