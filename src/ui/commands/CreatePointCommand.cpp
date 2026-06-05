#include "CreatePointCommand.h"

CreatePointCommand::CreatePointCommand(SceneAdapter *adapter, double x, double y)
: m_adapter(adapter), m_x(x), m_y(y)
{}

void CreatePointCommand::execute() {
    m_point = m_adapter->geoScene()->create<Point>(m_x, m_y);
    m_adapter->addPoint(m_point);
}

void CreatePointCommand::undo() {
    m_adapter->remove(m_point);
    m_point = nullptr;
}