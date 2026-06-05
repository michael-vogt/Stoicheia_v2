#include "CreateCircleCommand.h"

CreateCircleCommand::CreateCircleCommand(SceneAdapter *adapter, Point *p1, Point *p2)
: m_adapter(adapter), m_p1(p1), m_p2(p2)
{}

CreateCircleCommand::CreateCircleCommand(SceneAdapter *adapter, CreatePointCommand *p1Cmd, CreatePointCommand *p2Cmd, Point *p1, Point *p2)
    : m_adapter(adapter), m_p1Cmd(p1Cmd), m_p2Cmd(p2Cmd), m_p1(p1), m_p2(p2)
{}

void CreateCircleCommand::execute() {
    Point* p1 = resolveP1();
    Point* p2 = resolveP2();

    m_circle = m_adapter->geoScene()->create<Circle>(p1, p2);
    m_adapter->addCircle(m_circle);
}

void CreateCircleCommand::undo() {
    m_adapter->remove(m_circle);
    m_circle = nullptr;
}