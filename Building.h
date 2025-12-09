#ifndef THE_GLORBZ_BUILDING_H
#define THE_GLORBZ_BUILDING_H
#include "Position.h"
#include "Enums.h"

class Building {
    static int nextId;
    int buildingId;
    Position position;
    int hitPoints;
    //int owner;
    //BuildingType type;

public:
    Building(const Position &build_position, const BuildingType &build_type,
             const int hit_points, const int owner);
    [[nodiscard]] int getId() const;
    [[nodiscard]] Position getPosition() const;
    void takeDamage(int amount);
    [[nodiscard]] int getHitPoints() const;

};

#endif