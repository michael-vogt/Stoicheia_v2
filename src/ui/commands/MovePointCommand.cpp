#include "MovePointCommand.h"

MovePointCommand::MovePointCommand(Point *point, double new_x, double new_y)
: m_point(point), m_oldX(point->x()), m_oldY(point->y()), m_newX(new_x), m_newY(new_y)
{}

void MovePointCommand::execute() {
    m_point->moveTo(m_newX, m_newY);
}

void MovePointCommand::undo() {
    m_point->moveTo(m_oldX, m_oldY);
}

void MovePointCommand::setTarget(double new_x, double new_y) {
    m_newX = new_x;
    m_newY = new_y;
}