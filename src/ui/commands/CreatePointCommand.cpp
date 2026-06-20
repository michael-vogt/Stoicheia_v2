#include "CreatePointCommand.h"

CreatePointCommand::CreatePointCommand(SceneAdapter *adapter, double pos_x, double pos_y)
: m_adapter(adapter), m_x(pos_x), m_y(pos_y)
{}

void CreatePointCommand::execute() {
    if (m_point == nullptr) {
        // erstmaliges Ausführen: neuen Punkt erzeugen
        m_point = m_adapter->geoScene()->create<Point>(m_x, m_y);
    } else {
        // Redo: Punkt ist noch im Graveyard – zurückholen statt neu erzeugen,
        // damit alle Commands, die m_point kennen, denselben Pointer benutzen.
        m_adapter->geoScene()->restoreFromGraveyard(m_point);
    }
    m_adapter->addPoint(m_point);
}

void CreatePointCommand::undo() {
    m_adapter->remove(m_point);
    // m_point absichtlich NICHT auf nullptr setzen – für das nächste Redo.
    //m_point = nullptr;
}