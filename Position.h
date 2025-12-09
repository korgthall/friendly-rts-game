#ifndef THE_GLORBZ_POSITION_H
#define THE_GLORBZ_POSITION_H

class Position {
    int x;
    int y;

public:
    Position() = delete;
    Position(const int x, const int y);
    Position(const Position &position);

    /*
    void setX(int set_x);
    void setY(int set_y);
    */

    [[nodiscard]] int getX() const;
    [[nodiscard]] int getY() const;
    bool operator<(const Position& other) const;
    bool operator==(const Position& pos) const;
    bool operator!=(const Position& pos) const;
    Position& operator=(const Position&);
};

#endif