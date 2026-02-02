#include "../headers/Worker.h"
#include <iostream>

Worker::Worker(const Position &position, int hit_points, int attack_damage,
               const int attack_range, const int vision_range, int speed, int owner,
               const int total_capacity, const int current_capacity,
               const int gather_speed)
    : Unit(position, hit_points, attack_damage, attack_range, vision_range, speed, owner),
      toBuild(BuildingType::HUB), totalCapacity(total_capacity),
      currentCapacity(current_capacity), gatherSpeed(gather_speed),
      carriedType(ResourceType::GOLD), buildTarget(position), resourceTarget(position) {
}

void Worker::attack() { std::cout << "Worker " << id << " attacks!" << std::endl; }

void Worker::setBuildingProject(BuildingType type) { toBuild = type; }
BuildingType Worker::getBuildingProject() const { return toBuild; }
void Worker::setBuildTarget(const Position &target) { buildTarget = target; }
Position Worker::getBuildTarget() const { return buildTarget; }
void Worker::setResourceTarget(const Position &pos) { resourceTarget = pos; }
Position Worker::getResourceTarget() const { return resourceTarget; }
bool Worker::isFull() const { return currentCapacity >= totalCapacity; }
int Worker::getGatherSpeed() const { return gatherSpeed; }

void Worker::addResource(int amount, ResourceType type) {
    if (currentCapacity == 0) carriedType = type;
    if (carriedType == type) {
        currentCapacity += amount;
        if (currentCapacity > totalCapacity) currentCapacity = totalCapacity;
    }
}

int Worker::dumpResources() {
    int amount = currentCapacity;
    currentCapacity = 0;
    return amount;
}

ResourceType Worker::getCarriedType() const { return carriedType; }
