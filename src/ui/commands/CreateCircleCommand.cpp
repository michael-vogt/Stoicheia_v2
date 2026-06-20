#include "CreateCircleCommand.h"
#include "Structs.h"

CreateCircleCommand::CreateCircleCommand(SceneAdapter *adapter, PointPairForCircle points)
: m_adapter(adapter), m_center(points.center), m_radiusPoint(points.radiusPoint)
{}

CreateCircleCommand::CreateCircleCommand(SceneAdapter *adapter, CreatePointCommandPairForCircle cmds, PointPairForCircle points)
    : m_adapter(adapter), m_p1Cmd(cmds.centerPointCmd), m_p2Cmd(cmds.radiusPointCmd), m_center(points.center), m_radiusPoint(points.radiusPoint)
{}

void CreateCircleCommand::execute() {
    Point* point1 = resolvePoint1();
    Point* point2 = resolvePoint2();

    if (m_circle == nullptr) {
        m_circle = m_adapter->geoScene()->create<Circle>(PointPairForCircle{.center=point1, .radiusPoint=point2});
    } else {
        m_adapter->geoScene()->restoreFromGraveyardWithSources(m_circle, {point1, point2});
    }
    m_adapter->addCircle(m_circle);
}

void CreateCircleCommand::undo() {
    m_adapter->remove(m_circle);
    // m_circle absichtlich NICHT auf nullptr – für das nächste Redo.
    //m_circle = nullptr;
}