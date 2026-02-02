#ifndef SOLDIER_H
#define SOLDIER_H

#include "Unit.h"

class Soldier : public Unit {
public:
    Soldier(const Position &pos, int owner);

    void attack() override;
};

#endif
