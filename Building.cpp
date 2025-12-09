#include "Building.h"

int Building::nextId = 10000;

Building::Building(const Position &build_position, const BuildingType &build_type,
                   const int hit_points, const int owner)
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

