#ifndef STATE_H
#define STATE_H

class Unit;
class GameEngine;

class State {
public:
    virtual ~State() = default;

    virtual void update(Unit *unit, GameEngine *game, float deltaTime) = 0;

    virtual void onEnter([[maybe_unused]] Unit *unit) {
    }

    virtual void onExit([[maybe_unused]] Unit *unit) {
    }
};

#endif
