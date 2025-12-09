#ifndef RESOURCE_H
#define RESOURCE_H
#include "Position.h"
#include "Enums.h"

class Resource {
    [[maybe_unused]] int amount;
    static int nextId;
    int resourceId;
    Position position;
    [[maybe_unused]] ResourceType type;

public:
    Resource(const int amount, const Position &position, ResourceType type);
};

#endif