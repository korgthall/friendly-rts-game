#include "Unit.h"

int Unit::nextId = 1;

Unit::Unit(const Position& position, const int hit_points, const int attack_damage,
           const int attack_range, const int vision_range, const int speed, const int owner)
    : position(position), attackTargetPosition(position), unitId(nextId++), hitPoints(hit_points),
      attackDamage(attack_damage), attackRange(attack_range),
      visionRange(vision_range), speed(speed), owner(owner) {
}

Unit &Unit::operator=(const Position &new_position) {
    position = new_position;
    return *this;
}
int Unit::getAttackDamage()const{return attackDamage;}
[[nodiscard]] int Unit::getHitPoints() const { return hitPoints; }
void Unit::takeDamage(int amount) {
    hitPoints -= amount;
    if (hitPoints < 0) {
        hitPoints = 0;
    }
}
[[nodiscard]] Position Unit::getPosition() const { return position; }
[[nodiscard]] int Unit::getUnitId() const { return unitId; }

void Unit::setPath(const std::vector<Position>& newPath) {
    path = newPath;
}

bool Unit::hasPath() const {
    return !path.empty();
}

Position Unit::getNextStep() const {
    if (path.empty()) return position;
    return path.front();
}

void Unit::advancePath() {
    if (!path.empty()) {
        path.erase(path.begin());
    }
}