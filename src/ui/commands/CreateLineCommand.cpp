#include "CreateLineCommand.h"

CreateLineCommand::CreateLineCommand(SceneAdapter *adapter, Point *p1, Point *p2, LinearObjectType type) : m_adapter(adapter), m_p1(p1), m_p2(p2), m_type(type) {}

void CreateLineCommand::execute() {
    switch (m_type) {
        case LinearObjectType::Line:
            m_linear = m_adapter->geoScene()->create<Line>(m_p1, m_p2);
            break;
        case LinearObjectType::Ray:
            m_linear = m_adapter->geoScene()->create<Ray>(m_p1, m_p2);
            break;
        case LinearObjectType::Segment:
            m_linear = m_adapter->geoScene()->create<Segment>(m_p1, m_p2);
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
