#include "MacroCommand.h"

MacroCommand::MacroCommand(const QString &description) : m_description(description) {}

void MacroCommand::add(std::unique_ptr<Command> cmd) {
    m_commands.push_back(std::move(cmd));
}

void MacroCommand::execute() {
    for (const auto& cmd: m_commands)
        cmd->execute();
}

void MacroCommand::undo() {
    for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it)
        (*it)->undo();
}

