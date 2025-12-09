#include "Position.h"

Position::Position(const int x, const int y) {
    this->x = x;
    this->y = y;
}

Position::Position(const Position &position) {
    x = position.x;
    y = position.y;
}

/*
void Position::setX(int set_x) {
    x = set_x;
}

void Position::setY(int set_y) {
    y = set_y;
}
*/

[[nodiscard]] int Position::getX() const {
    return x;
}

[[nodiscard]] int Position::getY() const {
    return y;
}
bool Position::operator==(const Position& pos) const {
    return x == pos.x && y == pos.y;
}

bool Position::operator<(const Position& pos) const {
    if (x != pos.x) return x < pos.x;
    return y < pos.y;
}

bool Position::operator!=(const Position& pos) const {
    return !(x == pos.x && y == pos.y);
}
constexpr Position& Position::operator=(const Position& p){
    x=p.x;
    y=p.y;
    return *this;
}