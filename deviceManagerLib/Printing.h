#pragma once
#include "DeviceNode.h"

class IndentGuard {
public:
    IndentGuard();
    IndentGuard(const IndentGuard&);
    ~IndentGuard();
};

void free_printer(DeviceNode& devNode);
