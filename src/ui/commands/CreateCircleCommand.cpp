#include "CreateCircleCommand.h"
#include "Structs.h"

CreateCircleCommand::CreateCircleCommand(SceneAdapter *adapter, PointPairForCircle points)
: m_adapter(adapter), m_center(points.center), m_radiusPoint(points.radiusPoint)
{}

CreateCircleCommand::CreateCircleCommand(SceneAdapter *adapter, CreatePointCommandPairForCircle cmds, PointPairForCircle points)
    : m_adapter(adapter), m_p1Cmd(cmds.centerPointCmd), m_p2Cmd(cmds.radiusPointCmd), m_center(points.center), m_radiusPoint(points.radiusPoint)
{}

void CreateCircleCommand::execute() {
    Point* point1 = resolveP1();
    Point* point2 = resolveP2();

    m_circle = m_adapter->geoScene()->create<Circle>(PointPairForCircle{.center=point1, .radiusPoint=point2});
    m_adapter->addCircle(m_circle);
}

void CreateCircleCommand::undo() {
    m_adapter->remove(m_circle);
    m_circle = nullptr;
}