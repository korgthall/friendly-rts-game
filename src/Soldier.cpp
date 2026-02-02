#include "../headers/Soldier.h"
#include <iostream>

Soldier::Soldier(const Position &pos, int owner) : Unit(pos, 200, 25, 2, 8, 3, owner) {
}

void Soldier::attack() { std::cout << "Soldier fires!" << std::endl; }

