#include "Worker.h"

Worker::Worker(const Position &position, const int hit_points, const int attack_damage,
               const int attack_range, const int vision_range, const int speed, const int owner,
               const Tasks task, const int total_capacity, const int current_capacity,
               const int gather_speed) : Unit(position, hit_points, attack_damage, attack_range, vision_range, speed,
                                              owner),
                                         task(task), toBuild(BuildingType::HUB), totalCapacity(total_capacity), currentCapacity(current_capacity),
                                         gatherSpeed(gather_speed), buildTarget(position) {
}


void Worker::setTask(Tasks new_task) { task = new_task; }
[[nodiscard]] Tasks Worker::getTask() const { return task; }
/* bool Worker::isFull() const { return currentCapacity >= totalCapacity; } */
void Worker::setBuildingProject(BuildingType type) { toBuild = type; }
BuildingType Worker::getBuildingProject() const { return toBuild; }
int Worker::getOwner() const { return owner; }
void Worker::attack() {
    std::cout << "Worker " << unitId << " swings pickaxe at target!" << std::endl;
}
void Worker::setAttackTarget(const Position& target) { attackTargetPosition = target; }
Position Worker::getAttackTarget() const { return attackTargetPosition; }
void Worker::setBuildTarget(const Position& target) { buildTarget = target; }
Position Worker::getBuildTarget() const { return buildTarget; }