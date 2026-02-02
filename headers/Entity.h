#ifndef ENTITY_H
#define ENTITY_H

#include "Position.h"

class Entity {
protected:
    static int nextId;
    int id;
    Position position;
    int hitPoints;
    int owner;

public:
    Entity(const Position &pos, int hp, int owner);

    virtual ~Entity() = default;

    [[nodiscard]] int getId() const;

    [[nodiscard]] Position getPosition() const;

    [[nodiscard]] int getOwner() const;

    [[nodiscard]] int getHitPoints() const;

    virtual void takeDamage(int amount);

    virtual void setPosition(const Position &pos);
};

#endif
