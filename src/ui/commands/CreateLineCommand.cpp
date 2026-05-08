#include "CreateLineCommand.h"

CreateLineCommand::CreateLineCommand(SceneAdapter *adapter, Point *p1, Point *p2) : m_adapter(adapter), m_p1(p1), m_p2(p2) {}

void CreateLineCommand::execute() {
    m_line = m_adapter->geoScene()->create<Line>(m_p1, m_p2);
    m_adapter->addLinearObject(m_line);
}

void CreateLineCommand::undo() {
    m_adapter->remove(m_line);
    m_line = nullptr;
}

QString CreateLineCommand::description() const {
    return "Linie erstellen";
}
