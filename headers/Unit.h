#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"
#include "Enums.h"
#include <vector>
#include <memory>
#include "TaskQueue.h"

class State;
class GameEngine;

class Unit : public Entity {
protected:
    std::shared_ptr<State> currentState;
    Position attackTargetPosition;
    int attackDamage;
    int attackRange;
    int visionRange;
    int speed;

    TaskQueue<Position> pathQueue;

public:
    Unit(const Position &position, int hit_points, int attack_damage,
         const int attack_range, const int vision_range, int speed, int owner);

    void setState(std::shared_ptr<State> newState);

    void update(GameEngine *game, float deltaTime);

    std::shared_ptr<State> getCurrentState() const { return currentState; }

    void setPath(const std::vector<Position> &newPath);

    bool hasPath() const;

    Position getNextStep() const;

    void advancePath();

    void setAttackTarget(const Position &target);

    Position getAttackTarget() const;

    virtual void attack() = 0;

    virtual bool canHarvest() const { return false; }
    virtual bool canBuild() const { return false; }

    int getAttackDamage() const;

    Unit &operator=(const Position &new_position);
};

#endif
