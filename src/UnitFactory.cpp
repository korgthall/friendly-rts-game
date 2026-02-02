#include "../headers/UnitFactory.h"
#include "../headers/Worker.h"
#include "../headers/Soldier.h"

std::unique_ptr<Unit> UnitFactory::createUnit(UnitType type, Position pos, int owner) {
    switch (type) {
        case UnitType::SOLDIER:
            return std::make_unique<Soldier>(pos, owner);

        case UnitType::WORKER:
            return std::make_unique<Worker>(pos, 100, 5, 1, 5, 1, owner, 50, 0, 10);

        default:
            return nullptr;
    }
}
