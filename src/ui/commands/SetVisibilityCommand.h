#pragma once
#include "Command.h"

class SceneAdapter;
class GeoObject;

// Blendet ein geometrisches Objekt ein oder aus (undo-fähig).
class SetVisibilityCommand : public Command {
public:
    SetVisibilityCommand(SceneAdapter* adapter, GeoObject* object, bool visible);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override {
        return m_visible ? tr("Objekt einblenden") : tr("Objekt ausblenden");
    }

private:
    SceneAdapter* m_adapter;
    GeoObject*    m_object;
    bool          m_visible;
};