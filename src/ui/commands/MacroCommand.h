#pragma once
#include "Command.h"


class MacroCommand : public Command {
    QString m_description;
    std::vector<std::unique_ptr<Command>> m_commands;

public:
    explicit MacroCommand(const QString& description);

    void add(std::unique_ptr<Command> cmd);

    void execute() override;
    void undo() override;
    QString description() const override { return m_description; };
};
