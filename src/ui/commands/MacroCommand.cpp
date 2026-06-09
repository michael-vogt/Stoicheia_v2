#include "MacroCommand.h"

#include <ranges>

MacroCommand::MacroCommand(QString description)
: m_description(std::move(description))
{}

void MacroCommand::add(std::unique_ptr<Command> cmd) {
    m_commands.push_back(std::move(cmd));
}

void MacroCommand::execute() {
    for (const auto& cmd: m_commands) {
        cmd->execute();
    }
}

void MacroCommand::undo() {
    for (auto & m_command : std::views::reverse(m_commands)) {
        m_command->undo();
    }
}

