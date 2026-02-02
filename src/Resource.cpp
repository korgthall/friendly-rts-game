#include "../headers/Resource.h"

Resource::Resource(const Position &position, int amount, ResourceType type)
    : Entity(position, 1, 0), amount(amount), type(type) {
}

int Resource::harvest(int harvestAmount) {
    if (amount >= harvestAmount) {
        amount -= harvestAmount;
        return harvestAmount;
    } else {
        int remaining = amount;
        amount = 0;
        return remaining;
    }
}

bool Resource::isDepleted() const { return amount <= 0; }
ResourceType Resource::getResourceType() const { return type; }
