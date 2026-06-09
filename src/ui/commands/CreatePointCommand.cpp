#include "CreatePointCommand.h"

CreatePointCommand::CreatePointCommand(SceneAdapter *adapter, double pos_x, double pos_y)
: m_adapter(adapter), m_x(pos_x), m_y(pos_y)
{}

void CreatePointCommand::execute() {
    m_point = m_adapter->geoScene()->create<Point>(m_x, m_y);
    m_adapter->addPoint(m_point);
}

void CreatePointCommand::undo() {
    m_adapter->remove(m_point);
    m_point = nullptr;
}