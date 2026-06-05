#include "CreatePerpendicularFootCommand.h"
#include "../../constructions/PerpendicularFoot.h"

CreatePerpendicularFootCommand::CreatePerpendicularFootCommand(SceneAdapter* adapter, Point* point, LinearObject* line)
    : m_adapter(adapter), m_point(point), m_line(line)
{}

void CreatePerpendicularFootCommand::execute() {
    auto* foot = m_adapter->geoScene()->create<PerpendicularFoot>(m_point, m_line);
    m_adapter->addPoint(foot);
    m_result = foot;
}

void CreatePerpendicularFootCommand::undo() {
    if (m_result) {
        m_adapter->remove(m_result);
        m_result = nullptr;
    }
}