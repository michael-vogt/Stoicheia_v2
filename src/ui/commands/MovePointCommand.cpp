#include "MovePointCommand.h"

MovePointCommand::MovePointCommand(Point *point, double newX, double newY) : m_point(point), m_oldX(point->x()), m_oldY(point->y()), m_newX(newX), m_newY(newY)
{}

void MovePointCommand::execute() {
    m_point->moveTo(m_newX, m_newY);
}

void MovePointCommand::undo() {
    m_point->moveTo(m_oldX, m_oldY);
}

void MovePointCommand::setTarget(double x, double y) {
    m_newX = x;
    m_newY = y;
}

QString MovePointCommand::description() const {
    return QString("Punkt verschieben nach (%1, %2)").arg(m_newX).arg(m_newY);
}
