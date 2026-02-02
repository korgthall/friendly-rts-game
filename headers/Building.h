#ifndef BUILDING_H
#define BUILDING_H

#include "Entity.h"
#include "Enums.h"
#include "TaskQueue.h"

class Building : public Entity {
private:
    BuildingType type;
    TaskQueue<UnitType> productionQueue;
    float productionTimer;
    float productionSpeed;
    int storedResources;

public:
    Building(const Position &pos, const BuildingType &type, int hp, int owner);

    BuildingType getType() const;

    void queueUnit(UnitType uType);

    bool isProducing() const;

    void update(float deltaTime);

    bool isProductionComplete();

    UnitType popProduction();

    int getQueueSize() const;

    void setStoredResources(int amount);

    int harvest(int amount);
};

#endif
