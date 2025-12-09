#include "GameEngine.h"
#include "Worker.h"
#include <cmath>
#include <iostream>

GameEngine* GameEngine::instancePtr = nullptr;

GameEngine* GameEngine::getInstance() {
    if (instancePtr == nullptr) {
        instancePtr = new GameEngine();
    }
    return instancePtr;
}

void GameEngine::cleanup() {
    if (instancePtr != nullptr) {
        delete instancePtr;
        instancePtr = nullptr;
    }
}

void GameEngine::StartGame(sf::RenderWindow& window) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            tiles[i][j] = 0;
        }
    }

    sf::Vector2f windowSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    gameView.setSize(windowSize);
    gameView.setCenter({windowSize.x / 2.0f, windowSize.y / 2.0f});

    moveTimer = 0.0f;
    cameraSpeed = 500.0f;
    playerGold = 500;

    Position workerPos(5, 5);
    addWorker(workerPos, 1);

    std::cout << "Game Started. Gold: " << playerGold << std::endl;
}

void GameEngine::handleInput(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (!event) return;

    if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::B) {
            isBuildingMode = !isBuildingMode;
            isAttackingMode = false;
            std::cout << (isBuildingMode ? "Build Mode ON" : "Build Mode OFF") << std::endl;
        }
        else if (key->code == sf::Keyboard::Key::A) {
            isAttackingMode = !isAttackingMode;
            isBuildingMode = false;
            std::cout << (isAttackingMode ? "Attack Mode ON" : "Attack Mode OFF") << std::endl;
        }
    }

    if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Right) {

            window.setView(gameView);
            sf::Vector2f worldPos = window.mapPixelToCoords(mouse->position);
            int gridX = static_cast<int>(worldPos.x / TILE_SIZE);
            int gridY = static_cast<int>(worldPos.y / TILE_SIZE);

            if (gridX < 0 || gridX >= SIZE || gridY < 0 || gridY >= SIZE) return;

            if (!units.empty()) {
                Worker* worker = dynamic_cast<Worker*>(units[0].get());

                if (worker) {
                    try {
                        if (isBuildingMode) {
                            Position targetPos(gridX, gridY);
                            setUnitPath(worker, targetPos, true);

                            worker->setTask(Tasks::BUILD);
                            worker->setBuildingProject(BuildingType::BARRACKS);
                            worker->setBuildTarget(targetPos);

                            std::cout << "Worker ordered to build at: " << gridX << ", " << gridY << std::endl;
                            isBuildingMode = false;
                        }
                        else if (isAttackingMode) {
                            Position targetPos(gridX, gridY);
                            setUnitPath(worker, targetPos, true);

                            worker->setTask(Tasks::ATTACK);
                            worker->setAttackTarget(targetPos);

                            std::cout << "Worker ordered to attack at: " << gridX << ", " << gridY << std::endl;
                            isAttackingMode = false;
                        }
                        else {
                            setUnitPath(worker, Position(gridX, gridY), false);
                            worker->setTask(Tasks::MOVING);
                            std::cout << "Worker moving to: " << gridX << ", " << gridY << std::endl;
                        }
                    }
                    catch (const PathNotFoundException& e) {
                        std::cerr << "[Command Failed]: " << e.what() << std::endl;
                    }
                }
            }
        }
    }
}

void GameEngine::Update(float deltaTime) {
    moveTimer += deltaTime;
    if (moveTimer > 0.1f) {

        for (auto& unit : units) {
            Worker* worker = dynamic_cast<Worker*>(unit.get());

            if (unit->hasPath()) {
                Position nextStep = unit->getNextStep();

                if (isValidMove(nextStep.getX(), nextStep.getY())) {
                    moveUnit(unit.get(), nextStep);
                    unit->advancePath();
                } else {
                    unit->setPath({});
                }
            }
            else if (worker) {
                if (worker->getTask() == Tasks::BUILD) {
                    Position currentPos = worker->getPosition();
                    Position targetPos = worker->getBuildTarget();

                    int dx = std::abs(currentPos.getX() - targetPos.getX());
                    int dy = std::abs(currentPos.getY() - targetPos.getY());

                    if (dx <= 1 && dy <= 1) {

                        try {
                            if (currentPos == targetPos) {
                                throw TileOccupiedException(targetPos.getX(), targetPos.getY());
                            }
                            for(const auto& b : buildings) {
                                if(b->getPosition() == targetPos)
                                    throw TileOccupiedException(targetPos.getX(), targetPos.getY());
                            }

                            int buildCost = (worker->getBuildingProject() == BuildingType::HUB) ? 1000 : 100;
                            if (playerGold < buildCost) {
                                throw InsufficientFundsException(buildCost, playerGold);
                            }

                            playerGold -= buildCost;
                            addBuilding(targetPos, worker->getBuildingProject(), worker->getOwner());
                            std::cout << "Construction Complete! Gold Remaining: " << playerGold << std::endl;
                            worker->setTask(Tasks::IDLE);

                        } catch (const GameException& e) {
                            std::cerr << "[Game Logic Error]: " << e.what() << std::endl;
                            worker->setTask(Tasks::IDLE);
                        }
                    }
                }
                else if (worker->getTask() == Tasks::ATTACK) {
                    Position currentPos = worker->getPosition();
                    Position targetPos = worker->getAttackTarget();

                    int dx = std::abs(currentPos.getX() - targetPos.getX());
                    int dy = std::abs(currentPos.getY() - targetPos.getY());

                    if (dx <= 1 && dy <= 1) {
                        resolveCombat(worker, targetPos);
                        worker->setTask(Tasks::IDLE);
                    }
                }
            }
        }
        moveTimer = 0.0f;
    }
}

void GameEngine::resolveCombat(const Unit* attacker, const Position& targetPos) {
    int targetX = targetPos.getX();
    int targetY = targetPos.getY();

    if (targetX < 0 || targetX >= SIZE || targetY < 0 || targetY >= SIZE) return;

    int entityId = tiles[targetX][targetY];
    if (entityId == 0) {
        std::cout << "Attack Missed: Tile Empty." << std::endl;
        return;
    }

    int damage = attacker->getAttackDamage();

    for (auto it = units.begin(); it != units.end(); ++it) {
        if ((*it)->getUnitId() == entityId && (*it)->getPosition() == targetPos) {
            (*it)->takeDamage(damage);
            std::cout << "HIT Unit " << entityId << "! HP: " << (*it)->getHitPoints() << std::endl;
            if ((*it)->getHitPoints() <= 0) {
                std::cout << "Unit " << entityId << " destroyed!" << std::endl;
                tiles[targetX][targetY] = 0;
                units.erase(it);
            }
            return;
        }
    }

    for (auto it = buildings.begin(); it != buildings.end(); ++it) {
        if ((*it)->getId() == entityId && (*it)->getPosition() == targetPos) {
            (*it)->takeDamage(damage);
            std::cout << "HIT Building " << entityId << "! HP: " << (*it)->getHitPoints() << std::endl;
            if ((*it)->getHitPoints() <= 0) {
                std::cout << "Building " << entityId << " destroyed!" << std::endl;
                tiles[targetX][targetY] = 0;
                buildings.erase(it);
            }
            return;
        }
    }
}

void GameEngine::render(sf::RenderWindow& window) {
    window.setView(gameView);

    sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    tileShape.setOutlineThickness(1.0f);
    tileShape.setOutlineColor(sf::Color(50, 50, 50));

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            tileShape.setPosition(sf::Vector2f(x * TILE_SIZE, y * TILE_SIZE));
            tileShape.setFillColor(sf::Color(34, 139, 34));
            window.draw(tileShape);
        }
    }

    sf::RectangleShape buildingShape(sf::Vector2f(TILE_SIZE - 4, TILE_SIZE - 4));
    buildingShape.setFillColor(sf::Color::Red);

    for (const auto& building : buildings) {
        Position pos = building->getPosition();
        buildingShape.setPosition(sf::Vector2f(pos.getX() * TILE_SIZE + 2, pos.getY() * TILE_SIZE + 2));
        window.draw(buildingShape);
    }

    float unitRadius = TILE_SIZE / 2.0f - 4.0f;
    sf::CircleShape unitShape(unitRadius);
    unitShape.setFillColor(sf::Color::Blue);

    for (const auto& unit : units) {
        Position pos = unit->getPosition();
        unitShape.setPosition(sf::Vector2f(pos.getX() * TILE_SIZE + 4, pos.getY() * TILE_SIZE + 4));
        window.draw(unitShape);
    }
}

/*
int GameEngine::EndGame() { return 0; }
bool GameEngine::GameShouldEnd() const { return false; }
*/

bool GameEngine::isValidMove(int x, int y) const {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return false;
    return tiles[x][y] == 0;
}

std::vector<Position> GameEngine::findPath(Position start, Position target) {
    std::vector<Position> path;

    if (target.getX() < 0 || target.getX() >= SIZE || target.getY() < 0 || target.getY() >= SIZE) {
        return path;
    }

    if (start == target) return path;

    std::queue<Position> frontier;
    frontier.push(start);

    std::map<Position, std::shared_ptr<Position>> parentMap;
    parentMap[start] = nullptr;

    bool found = false;

    while (!frontier.empty()) {
        Position current = frontier.front();
        frontier.pop();

        if (current == target) {
            found = true;
            break;
        }

        const int dx[] = {0, 0, -1, 1};
        const int dy[] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++) {
            Position next(current.getX() + dx[i], current.getY() + dy[i]);

            bool isWalkable = isValidMove(next.getX(), next.getY());
            bool isTarget = (next == target);

            if ((isWalkable || isTarget) && parentMap.find(next) == parentMap.end()) {
                frontier.push(next);
                parentMap[next] = std::make_shared<Position>(current);
            }
        }
    }

    if (found) {
        Position step = target;
        while (parentMap[step] != nullptr) {
            path.push_back(step);
            step = *parentMap[step];
        }
        std::reverse(path.begin(), path.end());
    }
    return path;
}

void GameEngine::setUnitPath(Unit* unit, const Position& target, bool stopAdjacent) {
    std::vector<Position> path = findPath(unit->getPosition(), target);

    if (path.empty()) {
        if (!(unit->getPosition() == target)) {
             throw PathNotFoundException(target.getX(), target.getY());
        }
    }

    if (!path.empty() && stopAdjacent) {
        if (path.back() == target) {
            path.pop_back();
        }
    }

    unit->setPath(path);
}

void GameEngine::moveUnit(Unit* unit, const Position& new_position) {
    tiles[unit->getPosition().getX()][unit->getPosition().getY()] = 0;
    tiles[new_position.getX()][new_position.getY()] = unit->getUnitId();
    *unit = new_position;
}

void GameEngine::addWorker(const Position& pos, int owner) {
    if (isValidMove(pos.getX(), pos.getY())) {
        units.push_back(std::make_unique<Worker>(
            pos, 100, 100, 1, 5, 2, owner, Tasks::GATHERGOLD, 100, 0, 10
        ));
        tiles[pos.getX()][pos.getY()] = units.back()->getUnitId();
    }
}

void GameEngine::addBuilding(const Position &build_position, const BuildingType &build_type, int owner) {
    int hitPoints = (build_type == HUB) ? 1000 : 500;
    buildings.push_back(std::make_unique<Building>(build_position, build_type, hitPoints, owner));
    tiles[build_position.getX()][build_position.getY()] = buildings.back()->getId();
}
[[nodiscard]] std::unique_ptr<Unit>& GameEngine::getUnit(){return units[0];}
[[maybe_unused]] int GameEngine::getPlayerGold() const{return playerGold;}