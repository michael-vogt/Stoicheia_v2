#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class DeleteObjectCommand : public Command {

    Q_OBJECT

public:
    DeleteObjectCommand(SceneAdapter* adapter, GeoObject* object);

    void execute() override;
    void undo() override;
    QString description() const override { return tr("Objekt löschen"); }

private:
    std::function<void()> buildUndoFactory();

    SceneAdapter* m_adapter;
    GeoObject* m_object;
    std::function<void()> m_undoFactory;
};
