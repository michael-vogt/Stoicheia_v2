#pragma once
#include "Command.h"
#include "MoveCommand.h"
#include "geometry/Point.h"

class MovePointCommand : public MoveCommand {

    Q_OBJECT

public:
    MovePointCommand(Point* point, double newX, double newY);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Punkt verschieben nach (%1, %2)").arg(m_newX).arg(m_newY); }

    void setTarget(double x, double y) override;

private:
    Point* m_point;
    double m_oldX, m_oldY;
    double m_newX, m_newY;
};
