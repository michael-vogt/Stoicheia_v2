#include "CreateIntersectionCommand.h"

#include "constructions/CircleCircleIntersection.h"
#include "constructions/LineCircleIntersection.h"
#include "constructions/LineLineIntersection.h"

CreateIntersectionCommand::CreateIntersectionCommand(SceneAdapter* adapter, GeoObject* obj1, GeoObject* obj2)
    : m_adapter(adapter), m_obj1(obj1), m_obj2(obj2)
{}

void CreateIntersectionCommand::execute() {
    auto* linearObject1 = dynamic_cast<LinearObject*>(m_obj1);
    auto* linearObject2 = dynamic_cast<LinearObject*>(m_obj2);
    auto* circle1 = dynamic_cast<Circle*>(m_obj1);
    auto* circle2 = dynamic_cast<Circle*>(m_obj2);

    if ((linearObject1 != nullptr) && (linearObject2 != nullptr)) {
        m_result = m_adapter->geoScene()->create<LineLineIntersection>(linearObject1, linearObject2);
    } else if ((linearObject1 != nullptr) && (circle1 != nullptr)) {
        m_result = m_adapter->geoScene()->create<LineCircleIntersection>(linearObject1, circle2);
    } else if ((circle1 != nullptr) && (linearObject2 != nullptr)) {
        m_result = m_adapter->geoScene()->create<LineCircleIntersection>(linearObject2, circle1);
    } else if ((circle1 != nullptr) && (circle2 != nullptr)) {
        m_result = m_adapter->geoScene()->create<CircleCircleIntersection>(circle1, circle2);
    } else {
        return; // ungültige Kombination
    }

    m_adapter->addIntersectionSet(m_result);
}

void CreateIntersectionCommand::undo() {
    if (m_result != nullptr) {
        m_adapter->remove(m_result);
        m_result = nullptr;
    }
}