#ifndef WORKER_H
#define WORKER_H

#include "GameEngine.h"
#include "Unit.h"
#include "Enums.h"
#include <iostream>

class Worker : public Unit {
    Tasks task;
    BuildingType toBuild;
    //int totalCapacity;
    //int currentCapacity;
    //int gatherSpeed;
    Position buildTarget;
public:
    Worker(const Position &pos, int hp, int dmg, int range, int vision, int spd, int owner,
           Tasks t);
    void setTask(Tasks newTask);
    Tasks getTask() const;

    /* bool isFull() const; */

    void setBuildingProject(BuildingType type);
    BuildingType getBuildingProject() const;
    int getOwner() const;
    void attack() override;
    void setAttackTarget(const Position& target);
    Position getAttackTarget() const;

    void setBuildTarget(const Position &target);

    Position getBuildTarget() const;
};

#endif