#pragma once

#include "Components.h"

class IAction {
public:
    virtual void MoveForward(void) = 0;
    virtual void MoveBack(void) = 0;
};

