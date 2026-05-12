#include "CreateMidpointCommand.h"

#include "constructions/Midpoint.h"

CreateMidpointCommand::CreateMidpointCommand(SceneAdapter* adapter, Point* p1, Point* p2) : m_adapter(adapter), m_p1(p1), m_p2(p2) {}

void CreateMidpointCommand::execute() {
    auto* mid = m_adapter->geoScene()->create<Midpoint>(m_p1, m_p2);
    m_adapter->addPoint(mid);
    m_result = mid;
}

void CreateMidpointCommand::undo() {
    m_adapter->remove(m_result);
    m_result = nullptr;
}

QString CreateMidpointCommand::description() const {
    return QObject::tr("Mittelpunkt erstellen");
}