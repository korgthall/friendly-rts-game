#ifndef UNITSTATES_H
#define UNITSTATES_H

#include "State.h"

class IdleState : public State {
public:
    void update(Unit *unit, GameEngine *game, float deltaTime) override;
};

class MovingState : public State {
public:
    void update(Unit *unit, GameEngine *game, float deltaTime) override;
};

class AttackState : public State {
public:
    void update(Unit *unit, GameEngine *game, float deltaTime) override;
};

class GatherState : public State {
public:
    void update(Unit *unit, GameEngine *game, float deltaTime) override;
};

class DepositState : public State {
public:
    void update(Unit *unit, GameEngine *game, float deltaTime) override;
};

class BuildState : public State {
public:
    void update(Unit *unit, GameEngine *game, float deltaTime) override;
};

#endif
