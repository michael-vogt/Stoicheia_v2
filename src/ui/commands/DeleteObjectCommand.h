#pragma once
#include "Command.h"
#include "ui/SceneAdapter.h"

class DeleteObjectCommand : public Command {
    SceneAdapter* m_adapter;
    GeoObject* m_object;
    std::function<void()> m_undoFactory;

    std::function<void()> buildUndoFactory();

public:
    DeleteObjectCommand(SceneAdapter* adapter, GeoObject* object);

    void execute() override;
    void undo() override;
    QString description() const override;
};
