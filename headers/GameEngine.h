#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Enums.h"
#include "Position.h"
#include "Exceptions.h"

class Unit;
class Building;
class Resource;

class GameEngine {
    static GameEngine *instancePtr;
    const int TILE_SIZE = 40;
    const int UI_HEIGHT = 100;

    bool isPlacing = false;
    BuildingType placementType = BuildingType::BARRACKS;
    int tiles[SIZE][SIZE]{};

    std::vector<std::unique_ptr<Unit> > units;
    std::vector<std::unique_ptr<Building> > buildings;
    std::vector<std::unique_ptr<Resource> > resources;

    sf::View gameView;
    sf::View uiView;
    sf::Font guiFont;
    sf::Text resourceText;

    std::vector<Unit *> selectedUnits;
    Building *selectedBuilding = nullptr;
    bool isDragging = false;
    sf::Vector2f dragStart;
    sf::Vector2f dragEnd;

    float moveTimer{0.0f};
    int playerGold{0};
    int playerGlorbium{0};
    bool isAttackMode = false;
    bool showBuildMenu = false;
    float totalGameTime{0.0f};
    int waveNumber{0};

    void spawnEnemyWave();

    GameEngine();

    Position findNearestFreeTile(Position center);

    void spawnUnit(UnitType type, Position pos, int owner);

public:
    template<typename T>
    T *findNearestEntity(Position startPos, const std::vector<std::unique_ptr<T> > &list, int ownerToCheck) {
        T *nearest = nullptr;
        int minDistance = 999999;
        for (const auto &entity: list) {
            if (entity->getOwner() == ownerToCheck) {
                int dist = std::abs(startPos.getX() - entity->getPosition().getX()) +
                           std::abs(startPos.getY() - entity->getPosition().getY());
                if (dist < minDistance) {
                    minDistance = dist;
                    nearest = entity.get();
                }
            }
        }
        return nearest;
    }

    const std::vector<std::unique_ptr<Building> > &getBuildings() const { return buildings; }

    GameEngine(const GameEngine &) = delete;

    GameEngine &operator=(const GameEngine &) = delete;

    static GameEngine *getInstance();

    static void cleanup();

    void StartGame(sf::RenderWindow &window);

    void handleInput(const std::optional<sf::Event> &event, sf::RenderWindow &window);

    void Update(float deltaTime);

    void render(sf::RenderWindow &window);

    bool isValidMove(int x, int y) const;

    void moveUnit(Unit *unit, const Position &new_position);

    void addBuilding(const Position &pos, const BuildingType &type, int owner);

    void addResource(const Position &pos, int amount, ResourceType type);

    void resolveCombat(const Unit *attacker, const Position &targetPos);

    std::vector<Position> findPath(Position start, Position target);

    void setUnitPath(Unit *unit, const Position &target, bool stopAdjacent = false);

    std::pair<int, ResourceType> harvestAt(Position pos, int amount);

    void depositGlobalResources(int amount, ResourceType type);

    Position findNearestBuilding(Position start, BuildingType type, int owner);
};

#endif
