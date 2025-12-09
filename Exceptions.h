#ifndef THE_GLORBZ_EXCEPTIONS_H
#define THE_GLORBZ_EXCEPTIONS_H
#include <exception>
#include <string>

class GameException : public std::exception {
protected:
    std::string message;

public:
    explicit GameException(const std::string& msg) : message(msg) {}
    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

class PathNotFoundException : public GameException {
public:
    PathNotFoundException(int x, int y)
        : GameException("Pathfinding Failed: Cannot reach (" + std::to_string(x) + ", " + std::to_string(y) + ")") {}
};

class InsufficientFundsException : public GameException {
public:
    InsufficientFundsException(int required, int available)
        : GameException("Insufficient Funds! Required: " + std::to_string(required) +
                        ", Available: " + std::to_string(available)) {}
};

class TileOccupiedException : public GameException {
public:
    TileOccupiedException(int x, int y)
        : GameException("Construction Failed: Tile (" + std::to_string(x) + ", " + std::to_string(y) + ") is occupied.") {}
};

#endif