#pragma once
#include "Command.h"


class MacroCommand : public Command {
public:
    explicit MacroCommand(QString description);

    void add(std::unique_ptr<Command> cmd);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return m_description; }

private:
    QString m_description;
    std::vector<std::unique_ptr<Command>> m_commands;
};
