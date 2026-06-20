#include "CreateLineCommand.h"
#include "Structs.h"
#include "geometry/Line.h"
#include "geometry/Ray.h"
#include "geometry/Segment.h"

CreateLineCommand::CreateLineCommand(SceneAdapter *adapter, PointPairForLinearObject points, LinearObjectType type) 
: m_adapter(adapter), m_point1(points.point1), m_point2(points.point2), m_type(type) {}

CreateLineCommand::CreateLineCommand(SceneAdapter *adapter, CreatePointCommandPairForLinearObject cmds, PointPairForLinearObject points, LinearObjectType type)
    : m_adapter(adapter), m_p1Cmd(cmds.firstPoint), m_p2Cmd(cmds.secondPoint), m_point1(points.point1), m_point2(points.point2), m_type(type)
{}

void CreateLineCommand::execute() {
    Point* point1 = resolvePoint1();
    Point* point2 = resolvePoint2();

    if (m_linear == nullptr) {
        switch (m_type) {
            case LinearObjectType::Line:
                m_linear = m_adapter->geoScene()->create<Line>(PointPairForLinearObject{.point1=point1, .point2=point2});
                break;
            case LinearObjectType::Ray:
                m_linear = m_adapter->geoScene()->create<Ray>(PointPairForLinearObject{.point1=point1, .point2=point2});
                break;
            case LinearObjectType::Segment:
                m_linear = m_adapter->geoScene()->create<Segment>(PointPairForLinearObject{.point1=point1, .point2=point2});
                break;
        }
    } else {
        // Redo: denselben Punkt aus dem Graveyard zurückholen
        m_adapter->geoScene()->restoreFromGraveyardWithSources(m_linear, {point1, point2});
    }

    m_adapter->addLinearObject(m_linear);
}

void CreateLineCommand::undo() {
    m_adapter->remove(m_linear);
    // m_linear absichtlich NICHT auf nullptr – für das nächste Redo.
    //m_linear = nullptr;
}

auto CreateLineCommand::description() const -> QString {
    switch (m_type) {
        case LinearObjectType::Line: return tr("Gerade erstellen");
        case LinearObjectType::Ray: return tr("Halbgerade erstellen");
        case LinearObjectType::Segment: return tr("Strecke erstellen");
    }
    return "";
}
