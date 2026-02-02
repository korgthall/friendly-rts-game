#ifndef RESOURCE_H
#define RESOURCE_H

#include "Entity.h"
#include "Enums.h"

class Resource : public Entity {
    int amount;
    ResourceType type;

public:
    Resource(const Position &position, int amount, ResourceType type);

    int harvest(int harvestAmount);

    bool isDepleted() const;

    ResourceType getResourceType() const;
};

#endif
