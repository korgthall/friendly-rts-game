#ifndef UNIT_H
#define UNIT_H
#include "Position.h"
#include <vector>

class Unit {
protected:
    Position position;
    Position attackTargetPosition;
    int unitId;
    int hitPoints;
    int attackDamage;
    int attackRange;
    int visionRange;
    int speed;
    int owner;

    static int nextId;
    std::vector<Position> path;

public:
    virtual ~Unit() = default;

    Unit(const Position& position, int hit_points, int attack_damage,
         int attack_range, int vision_range, int speed, int owner);

    virtual void attack() = 0;
    int getAttackDamage()const;
    void takeDamage(int amount);

    Position getPosition() const;

    int getUnitId() const;

    void setPath(const std::vector<Position> &newPath);

    bool hasPath() const;

    Position getNextStep() const;

    void advancePath();

    [[nodiscard]] int getHitPoints() const;
    Unit& operator=(const Position& new_position);


};

#endif // UNIT_H