#pragma once
#include "Command.h"
#include "MoveCommand.h"
#include "geometry/Point.h"

class MoveCenterCommand : public MoveCommand {

    Q_OBJECT

public:
    MoveCenterCommand(Point* centerPoint, Point* radiusPoint, double newX, double newY);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Mittelpunkt verschieben"); }

    void setTarget(double x, double y) override;

private:
    Point* m_centerPoint;
    Point* m_radiusPoint;

    double m_oldCenterX, m_oldCenterY;
    double m_newCenterX, m_newCenterY;

    double m_offsetX, m_offsetY;
};
