#ifndef WORKER_H
#define WORKER_H

#include "Unit.h"
#include "Enums.h"

class Worker : public Unit {
private:
    BuildingType toBuild;
    int totalCapacity;
    int currentCapacity;
    int gatherSpeed;
    ResourceType carriedType;
    Position buildTarget;
    Position resourceTarget;

public:
    Worker(const Position &position, int hit_points, int attack_damage,
           const int attack_range, const int vision_range, int speed, int owner,
           const int total_capacity, const int current_capacity,
           const int gather_speed);

    void attack() override;

    bool canHarvest() const override { return true; }
    bool canBuild() const override { return true; }

    void setBuildingProject(BuildingType type);

    BuildingType getBuildingProject() const;

    void setBuildTarget(const Position &target);

    Position getBuildTarget() const;

    void setResourceTarget(const Position &pos);

    Position getResourceTarget() const;

    bool isFull() const;

    void addResource(int amount, ResourceType type);

    int dumpResources();

    ResourceType getCarriedType() const;

    int getGatherSpeed() const;
};

#endif
