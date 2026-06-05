#pragma once

#include "Command.h"

class MoveCommand : public Command {
public:
    virtual void setTarget(double x, double y) = 0;
};
