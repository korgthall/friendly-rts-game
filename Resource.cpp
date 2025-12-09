#include "Resource.h"

int Resource::nextId = 1;

Resource::Resource(const int amount, const Position &position, ResourceType type)
    : amount(amount), resourceId(nextId++),
      position(position),
      type(type) {
}