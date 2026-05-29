#include "CreateParallelCommand.h"
#include "../../constructions/Parallel.h"

CreateParallelCommand::CreateParallelCommand(SceneAdapter* adapter, Point* origin, LinearObject* reference)
    : m_adapter(adapter), m_origin(origin), m_reference(reference)
{}

void CreateParallelCommand::execute() {
    auto* parallel = m_adapter->geoScene()->create<Parallel>(m_origin, m_reference);
    m_adapter->addLinearObject(parallel->line());
    m_result = parallel;
}

void CreateParallelCommand::undo() {
    if (m_result) {
        m_adapter->remove(m_result->line());
        m_result = nullptr;
    }
}

QString CreateParallelCommand::description() const {
    return tr("Parallele erstellen");
}