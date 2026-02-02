#include "../headers/Entity.h"

int Entity::nextId = 1;

Entity::Entity(const Position &pos, int hp, int owner)
    : id(nextId++), position(pos), hitPoints(hp), owner(owner) {
}

int Entity::getId() const { return id; }
Position Entity::getPosition() const { return position; }
int Entity::getOwner() const { return owner; }
int Entity::getHitPoints() const { return hitPoints; }

void Entity::takeDamage(int amount) {
    hitPoints -= amount;
    if (hitPoints < 0) hitPoints = 0;
}

void Entity::setPosition(const Position &
    pos) {
    position = pos;
}
