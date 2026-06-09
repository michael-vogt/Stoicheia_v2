#include "MoveCenterCommand.h"
#include <QObject>

MoveCenterCommand::MoveCenterCommand(Point *centerPoint, Point *radiusPoint, double new_x, double new_y)
: m_centerPoint(centerPoint), m_radiusPoint(radiusPoint),
  m_oldCenterX(centerPoint->x()), m_oldCenterY(centerPoint->y()),
  m_newCenterX(new_x), m_newCenterY(new_y),
  m_offsetX(radiusPoint->x() - centerPoint->x()), m_offsetY(radiusPoint->y() - centerPoint->y())
{}

void MoveCenterCommand::execute() {
    m_centerPoint->moveTo(m_newCenterX, m_newCenterY);
    m_radiusPoint->moveTo(m_newCenterX + m_offsetX, m_newCenterY + m_offsetY);
}

void MoveCenterCommand::undo() {
    m_centerPoint->moveTo(m_oldCenterX, m_oldCenterY);
    m_radiusPoint->moveTo(m_oldCenterX + m_offsetX, m_oldCenterY + m_offsetY);
}

void MoveCenterCommand::setTarget(const double new_x, const double new_y) {
    m_newCenterX = new_x;
    m_newCenterY = new_y;
}