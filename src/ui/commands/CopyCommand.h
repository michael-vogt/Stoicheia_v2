#pragma once
#include "Command.h"
#include "CreatePointCommand.h"
#include "geometry/GeoObject.h"


class CopyCommand : public Command {

    Q_OBJECT

public:
    CopyCommand(SceneAdapter* adapter, const std::unordered_set<GeoObject*>& selection, const QPointF& offset);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Kopieren"); };

    // Gibt die neu erzeugten Objekte zurück (nach execute())
    const std::vector<GeoObject*>& copies() const { return m_copies; }

private:
    // Löst einen Pointer auf: original -> Kopie (falls vorhanden)
    Point* resolvePoint(Point* p) const;
    LinearObject* resolveLinearObject(LinearObject* lo) const;
    Circle* resolveCircle(Circle* c) const;

    void createCopies();
    void removeCopies();

    SceneAdapter* m_adapter = nullptr;
    std::vector<GeoObject*> m_originals;
    std::vector<GeoObject*> m_copies;
    QPointF m_offset;

    // Mapping: original -> Kopie
    std::unordered_map<GeoObject*, GeoObject*> m_map;
};
