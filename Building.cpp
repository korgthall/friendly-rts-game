#include "Building.h"

int Building::nextId = 10000;

Building::Building(const Position &build_position,
                   const int hit_points)
    : buildingId(nextId++), position(build_position), hitPoints(hit_points){
}
[[nodiscard]] int Building::getId() const
{return buildingId;}
[[nodiscard]] Position Building::getPosition() const {return position;}
[[nodiscard]] int Building::getHitPoints() const { return hitPoints; }

void Building::takeDamage(int amount) {
    hitPoints -= amount;
    if (hitPoints < 0) {
        hitPoints = 0;
    }
}

