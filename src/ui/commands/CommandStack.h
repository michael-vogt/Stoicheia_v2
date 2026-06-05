#pragma once
#include <memory>
#include <vector>
#include <QObject>

#include "Command.h"


class CommandStack : public QObject {

    Q_OBJECT

public:
    explicit CommandStack(QObject *parent = nullptr) : QObject(parent) {};

    // Kommando ausführen
    void execute(std::unique_ptr<Command> command);
    void pushWithoutExecute(std::unique_ptr<Command> cmd);

    // Undo/Redo
    void undo();
    void redo();
    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }
    QString nextUndoDescription() const;
    QString nextRedoDescription() const;

    void clear();

signals:
    void changed();

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
};
