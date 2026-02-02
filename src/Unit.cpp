#include "../headers/Unit.h"
#include "../headers/State.h"
#include "../headers/UnitStates.h"

Unit::Unit(const Position &position, int hit_points, int attack_damage,
           const int attack_range, const int vision_range, int speed, int owner)
    : Entity(position, hit_points, owner),
      currentState(std::make_shared<IdleState>()),
      attackTargetPosition(position),
      attackDamage(attack_damage),
      attackRange(attack_range),
      visionRange(vision_range),
      speed(speed),
      pathQueue(100) {
}

void Unit::setState(std::shared_ptr<State> newState) {
    if (currentState) currentState->onExit(this);
    currentState = newState;
    if (currentState) currentState->onEnter(this);
}

void Unit::update(GameEngine *game, float deltaTime) {
    if (currentState) currentState->update(this, game, deltaTime);
}

void Unit::setPath(const std::vector<Position> &newPath) {
    pathQueue.loadFromVector(newPath);
}

bool Unit::hasPath() const {
    return !pathQueue.isEmpty();
}

Position Unit::getNextStep() const {
    return pathQueue.isEmpty() ? position : pathQueue.peek();
}

void Unit::advancePath() {
    pathQueue.pop();
}

void Unit::setAttackTarget(const Position &target) { attackTargetPosition = target; }
Position Unit::getAttackTarget() const { return attackTargetPosition; }
int Unit::getAttackDamage() const { return attackDamage; }

Unit &Unit::operator=(const Position &new_position) {
    this->position = new_position;
    return *this;
}
