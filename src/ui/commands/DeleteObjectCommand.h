#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class DeleteObjectCommand : public Command {

    Q_OBJECT

public:
    DeleteObjectCommand(SceneAdapter* adapter, GeoObject* object);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Objekt löschen"); }

private:
    auto buildUndoFactory() -> std::function<void()>;

    SceneAdapter* m_adapter;
    GeoObject* m_object;
    std::function<void()> m_undoFactory;
};
