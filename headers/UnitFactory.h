#ifndef UNITFACTORY_H
#define UNITFACTORY_H
#include <memory>
#include "Enums.h"
#include "Position.h"

class Unit;

class UnitFactory {
public:
    static std::unique_ptr<Unit> createUnit(UnitType type, Position pos, int owner);
};

#endif
