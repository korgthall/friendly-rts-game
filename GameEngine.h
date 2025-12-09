#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <algorithm>
#include <optional>
#include <SFML/Graphics.hpp>

#include "Unit.h"
#include "Worker.h"
#include "Building.h"
#include "Position.h"
#include "Exceptions.h"

class Worker;

class GameEngine {

    static GameEngine* instancePtr;
    const int TILE_SIZE = 40;
    int tiles[SIZE][SIZE]{};
    std::vector<std::unique_ptr<Unit>> units;
    std::vector<std::unique_ptr<Building>> buildings;

    sf::View gameView;
    float moveTimer{0.0f};
    float cameraSpeed{0.0f};

    int playerGold{0};
    bool isBuildingMode = false;
    bool isAttackingMode = false;

    GameEngine() = default;

    bool isValidMove(int x, int y) const;

public:
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    static GameEngine* getInstance();
    static void cleanup();

    void StartGame(sf::RenderWindow& window);

    void handleInput(const std::optional<sf::Event>& event, sf::RenderWindow& window);

    void Update(float deltaTime);

    void render(sf::RenderWindow& window);

    /*
    int EndGame();
    bool GameShouldEnd() const;
    */

    void addBuilding(const Position& build_position, const BuildingType& build_type, int owner);
    void addWorker(const Position& pos, int owner);

    void moveUnit(Unit* unit, const Position& new_position);

    void resolveCombat(const Unit* attacker, const Position& targetPos);

    std::vector<Position> findPath(Position start, Position target);

    void setUnitPath(Unit* unit, const Position& target, bool stopAdjacent = false);
    [[nodiscard]] std::unique_ptr<Unit>& getUnit();
    [[maybe_unused]] int getPlayerGold() const;
};

#endif