#pragma once
#include "Command.h"
#include "MoveCommand.h"
#include "geometry/Point.h"

class MoveCenterCommand : public MoveCommand {

    Q_OBJECT

public:
    MoveCenterCommand(Point* centerPoint, Point* radiusPoint, double new_x, double new_y);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Mittelpunkt verschieben"); }

    void setTarget(double new_x, double new_y) override;

private:
    Point* m_centerPoint;
    Point* m_radiusPoint;

    double m_oldCenterX, m_oldCenterY;
    double m_newCenterX, m_newCenterY;

    double m_offsetX, m_offsetY;
};
