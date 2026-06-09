#pragma once

#include "Command.h"

class MoveCommand : public Command {
public:
    virtual void setTarget(double new_x, double new_y) = 0;
};
