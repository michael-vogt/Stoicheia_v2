#pragma once
#include "Command.h"
#include "MoveCommand.h"
#include "geometry/Point.h"

class MovePointCommand : public MoveCommand {
    Point* m_point;
    double m_oldX, m_oldY;
    double m_newX, m_newY;

public:
    MovePointCommand(Point* point, double newX, double newY);

    void execute() override;
    void undo() override;
    QString description() const override;

    void setTarget(double x, double y) override;
};
