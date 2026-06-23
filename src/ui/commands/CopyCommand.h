#pragma once
#include "Command.h"
#include "geometry/GeoObject.h"
#include "ui/SceneAdapter.h"


class CopyCommand : public Command {

    Q_OBJECT

public:
    CopyCommand(SceneAdapter* adapter, const std::set<GeoObject*>& selection, const QPointF& offset);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Kopieren"); };

    // Gibt die neu erzeugten Objekte zurück (nach execute())
    [[nodiscard]] auto copies() const -> const std::vector<GeoObject*>& { return m_copies; }

private:
    // Löst einen Pointer auf: original -> Kopie (falls vorhanden)
    auto resolvePoint(Point* point) const -> Point*;
    auto resolveLinearObject(LinearObject* linearObject) const -> LinearObject*;
    auto resolveCircle(Circle* circle) const -> Circle*;

    void createCopies();
    void removeCopies();

    SceneAdapter* m_adapter = nullptr;
    std::vector<GeoObject*> m_originals;
    std::vector<GeoObject*> m_copies;
    QPointF m_offset;

    // Mapping: original -> Kopie
    std::map<GeoObject*, GeoObject*> m_map;
};
