#include "CreateMidpointCommand.h"

#include "Structs.h"
#include "constructions/Midpoint.h"

CreateMidpointCommand::CreateMidpointCommand(SceneAdapter* adapter, PointPairForLinearObject points) 
: m_adapter(adapter), m_point1(points.point1), m_point2(points.point2) 
{}

void CreateMidpointCommand::execute() {
    auto* mid = m_adapter->geoScene()->create<Midpoint>(m_point1, m_point2);
    m_adapter->addPoint(mid);
    m_result = mid;
}

void CreateMidpointCommand::undo() {
    m_adapter->remove(m_result);
    m_result = nullptr;
}