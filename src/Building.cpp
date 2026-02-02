#include "../headers/Building.h"

Building::Building(const Position &pos, const BuildingType &type, int hp, int owner)
    : Entity(pos, hp, owner), type(type), productionQueue(5), productionTimer(0.0f), productionSpeed(2.0f),
      storedResources(0) {
}

BuildingType Building::getType() const { return type; }
void Building::queueUnit(UnitType uType) { productionQueue.push(uType); }
bool Building::isProducing() const { return !productionQueue.isEmpty(); }

void Building::update(float deltaTime) {
    if (!productionQueue.isEmpty()) productionTimer += deltaTime;
}

bool Building::isProductionComplete() {
    return !productionQueue.isEmpty() && productionTimer >= productionSpeed;
}

UnitType Building::popProduction() {
    UnitType u = productionQueue.pop();
    productionTimer = 0.0f;
    return u;
}

int Building::getQueueSize() const { return productionQueue.size(); }
void Building::setStoredResources(int amount) { storedResources = amount; }

int Building::harvest(int amount) {
    if (type == BuildingType::GLORBSTRACTOR) return amount;
    return 0;
}
