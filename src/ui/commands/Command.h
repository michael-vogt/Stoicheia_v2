#pragma once
#include <qstring.h>

class Command {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;

    virtual QString description() const = 0;

};
