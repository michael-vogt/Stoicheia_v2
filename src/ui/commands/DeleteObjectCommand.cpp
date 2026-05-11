#include "DeleteObjectCommand.h"

DeleteObjectCommand::DeleteObjectCommand(SceneAdapter *adapter, GeoObject *object) : m_adapter(adapter), m_object(object)
{}

void DeleteObjectCommand::execute() {
    m_adapter->remove(m_object);
    m_object = nullptr;
}

void DeleteObjectCommand::undo() {
    // Löschen ist schwer rückgängig zu machen ohne das Objekt zu rekonstruieren.
    // Für eine vollständige Implementierung müsste hier der Zustand
    // des Objekts vor dem Löschen gespeichert und wiederhergestellt werden.
    // Vorerst: kein Undo für Delete.
}

QString DeleteObjectCommand::description() const {
    return "Objekt löschen";
}
