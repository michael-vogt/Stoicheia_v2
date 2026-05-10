#include "CreateLineCommand.h"

CreateLineCommand::CreateLineCommand(SceneAdapter *adapter, Point *p1, Point *p2, LinearObjectType type) : m_adapter(adapter), m_p1(p1), m_p2(p2), m_type(type) {}

CreateLineCommand::CreateLineCommand(SceneAdapter *adapter, CreatePointCommand *p1Cmd, CreatePointCommand *p2Cmd, Point *p1, Point *p2, LinearObjectType type)
    : m_adapter(adapter), m_p1Cmd(p1Cmd), m_p2Cmd(p2Cmd), m_p1(p1), m_p2(p2), m_type(type)
{}

Point *CreateLineCommand::resolveP1() const {
    return m_p1Cmd ? m_p1Cmd->point() : m_p1;
}

Point *CreateLineCommand::resolveP2() const {
    return m_p2Cmd ? m_p2Cmd->point() : m_p2;
}

void CreateLineCommand::execute() {
    Point* p1 = resolveP1();
    Point* p2 = resolveP2();

    switch (m_type) {
        case LinearObjectType::Line:
            m_linear = m_adapter->geoScene()->create<Line>(p1, p2);
            break;
        case LinearObjectType::Ray:
            m_linear = m_adapter->geoScene()->create<Ray>(p1, p2);
            break;
        case LinearObjectType::Segment:
            m_linear = m_adapter->geoScene()->create<Segment>(p1, p2);
            break;
    }

    m_adapter->addLinearObject(m_linear);
}

void CreateLineCommand::undo() {
    m_adapter->remove(m_linear);
    m_linear = nullptr;
}

QString CreateLineCommand::description() const {
    switch (m_type) {
        case LinearObjectType::Line: return "Gerade erstellen";
        case LinearObjectType::Ray: return "Halbgerade erstellen";
        case LinearObjectType::Segment: return "Strecke erstellen";
    }
    return "";
}
