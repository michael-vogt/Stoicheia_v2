#include "CreateIntersectionCommand.h"

#include "constructions/CircleCircleIntersection.h"
#include "constructions/LineCircleIntersection.h"
#include "constructions/LineLineIntersection.h"

CreateIntersectionCommand::CreateIntersectionCommand(SceneAdapter* adapter, GeoObject* obj1, GeoObject* obj2)
    : m_adapter(adapter), m_obj1(obj1), m_obj2(obj2)
{}

void CreateIntersectionCommand::execute() {
    auto* l1 = dynamic_cast<LinearObject*>(m_obj1);
    auto* l2 = dynamic_cast<LinearObject*>(m_obj2);
    auto* c1 = dynamic_cast<Circle*>(m_obj1);
    auto* c2 = dynamic_cast<Circle*>(m_obj2);

    if (l1 && l2) {
        m_result = m_adapter->geoScene()->create<LineLineIntersection>(l1, l2);
    } else if (l1 && c2) {
        m_result = m_adapter->geoScene()->create<LineCircleIntersection>(l1, c2);
    } else if (c1 && l2) {
        m_result = m_adapter->geoScene()->create<LineCircleIntersection>(l2, c1);
    } else if (c1 && c2) {
        m_result = m_adapter->geoScene()->create<CircleCircleIntersection>(c1, c2);
    } else {
        return; // ungültige Kombination
    }

    m_adapter->addIntersectionSet(m_result);
}

void CreateIntersectionCommand::undo() {
    if (m_result) {
        m_adapter->remove(m_result);
        m_result = nullptr;
    }
}