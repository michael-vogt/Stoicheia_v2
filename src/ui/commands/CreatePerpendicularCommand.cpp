#include "CreatePerpendicularCommand.h"
#include "../../constructions/Perpendicular.h"

CreatePerpendicularCommand::CreatePerpendicularCommand(SceneAdapter* adapter, Point* origin, LinearObject* reference)
    : m_adapter(adapter), m_origin(origin), m_reference(reference)
{}

void CreatePerpendicularCommand::execute() {
    auto* perp = m_adapter->geoScene()->create<Perpendicular>(m_origin, m_reference);
    m_adapter->addLinearObject(perp->line());
    m_result = perp;
}

void CreatePerpendicularCommand::undo() {
    if (m_result) {
        m_adapter->remove(m_result->line());
        m_result = nullptr;
    }
}

QString CreatePerpendicularCommand::description() const {
    return QObject::tr("Senkrechte erstellen");
}