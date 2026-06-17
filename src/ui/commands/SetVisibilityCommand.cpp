#include "SetVisibilityCommand.h"
#include "ui/SceneAdapter.h"

SetVisibilityCommand::SetVisibilityCommand(SceneAdapter* adapter, GeoObject* object, bool visible)
    : m_adapter(adapter), m_object(object), m_visible(visible)
{}

void SetVisibilityCommand::execute() {
    m_visible ? m_adapter->show(m_object) : m_adapter->hide(m_object);
}

void SetVisibilityCommand::undo() {
    m_visible ? m_adapter->hide(m_object) : m_adapter->show(m_object);
}