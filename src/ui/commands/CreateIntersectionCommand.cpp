#include "CreateIntersectionCommand.h"

#include "constructions/CircleCircleIntersection.h"
#include "constructions/LineCircleIntersection.h"
#include "constructions/LineLineIntersection.h"

CreateIntersectionCommand::CreateIntersectionCommand(SceneAdapter* adapter, GeoObject* obj1, GeoObject* obj2)
    : m_adapter(adapter), m_obj1(obj1), m_obj2(obj2)
{}

void CreateIntersectionCommand::execute() {
    auto* linear_object1 = dynamic_cast<LinearObject*>(m_obj1);
    auto* linear_object2 = dynamic_cast<LinearObject*>(m_obj2);
    auto* circle1 = dynamic_cast<Circle*>(m_obj1);
    auto* circle2 = dynamic_cast<Circle*>(m_obj2);

    if ((linear_object1 != nullptr) && (linear_object2 != nullptr)) {
        m_result = m_adapter->geoScene()->create<LineLineIntersection>(linear_object1, linear_object2);
    } else if ((linear_object1 != nullptr) && (circle1 != nullptr)) {
        m_result = m_adapter->geoScene()->create<LineCircleIntersection>(linear_object1, circle2);
    } else if ((circle1 != nullptr) && (linear_object2 != nullptr)) {
        m_result = m_adapter->geoScene()->create<LineCircleIntersection>(linear_object2, circle1);
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