#include "CommandStack.h"

void CommandStack::execute(std::unique_ptr<Command> command) {
    command->execute();
    m_undoStack.push_back(std::move(command));
    m_redoStack.clear();
    emit changed();
}

void CommandStack::pushWithoutExecute(std::unique_ptr<Command> cmd) {
    m_undoStack.push_back(std::move(cmd));
    m_redoStack.clear();
    emit changed();
}

void CommandStack::undo() {
    if (!canUndo()) {
        return;
    }
    auto& cmd = m_undoStack.back();
    cmd->undo();
    m_redoStack.push_back(std::move(cmd));
    m_undoStack.pop_back();
    emit changed();
}

void CommandStack::undoLast() {
    if (!canUndo()) {
        return;
    }

    m_undoStack.back()->undo();
    m_undoStack.pop_back();
    emit changed();
}

void CommandStack::redo() {
    if (!canRedo()) {
        return;
    }
    auto& cmd = m_redoStack.back();
    cmd->execute();
    m_undoStack.push_back(std::move(cmd));
    m_redoStack.pop_back();
    emit changed();
}

auto CommandStack::nextUndoDescription() const -> QString {
    return canUndo() ? m_undoStack.back()->description() : QString();
}

auto CommandStack::nextRedoDescription() const -> QString {
    return canRedo() ? m_redoStack.back()->description() : QString();
}

void CommandStack::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
    emit changed();
}
