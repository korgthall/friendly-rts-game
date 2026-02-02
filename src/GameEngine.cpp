#include "../headers/GameEngine.h"
#include "../headers/Unit.h"
#include "../headers/Worker.h"
#include "../headers/Soldier.h"
#include "../headers/Building.h"
#include "../headers/Resource.h"
#include "../headers/UnitFactory.h"
#include "../headers/UnitStates.h"
#include "../headers/Exceptions.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <queue>
#include <map>

GameEngine *GameEngine::instancePtr = nullptr;

GameEngine *GameEngine::getInstance() {
    if (instancePtr == nullptr) instancePtr = new GameEngine();
    return instancePtr;
}

void GameEngine::cleanup() {
    delete instancePtr;
    instancePtr = nullptr;
}

GameEngine::GameEngine()
    : resourceText(guiFont) {
}

void GameEngine::StartGame(sf::RenderWindow &window) {
    if (!guiFont.openFromFile("../assets/fonts/Comic_Relief/ComicRelief-Regular.ttf")) {
        std::cerr << "Error: Could not load font file." << std::endl;
    }

    resourceText.setFont(guiFont);
    resourceText.setCharacterSize(20);
    resourceText.setFillColor(sf::Color::White);
    resourceText.setOutlineColor(sf::Color::Black);
    resourceText.setOutlineThickness(1.0f);

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) tiles[i][j] = 0;

    sf::Vector2u winSize = window.getSize();
    sf::Vector2f winSizeF(static_cast<float>(winSize.x), static_cast<float>(winSize.y));

    gameView.setSize({winSizeF.x, winSizeF.y});
    gameView.setCenter({winSizeF.x / 2.0f, (winSizeF.y - UI_HEIGHT) / 2.0f});
    gameView.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));

    uiView.setSize({winSizeF.x, winSizeF.y});
    uiView.setCenter({winSizeF.x / 2.0f, winSizeF.y / 2.0f});

    moveTimer = 0.0f;
    playerGold = 800;
    playerGlorbium = 0;
    isAttackMode = false;

    spawnUnit(UnitType::WORKER, Position(3, 3), 1);
    addBuilding(Position(5, 5), BuildingType::HUB, 1);
    addResource(Position(8, 2), 5000, ResourceType::GOLD);
    addResource(Position(12, 6), 5000, ResourceType::GLORBIUM);
}

void GameEngine::spawnUnit(UnitType type, Position pos, int owner) {
    if (!isValidMove(pos.getX(), pos.getY())) return;
    std::unique_ptr<Unit> u = UnitFactory::createUnit(type, pos, owner);
    if (u) {
        tiles[pos.getX()][pos.getY()] = u->getId();
        units.push_back(std::move(u));
    }
}

void GameEngine::spawnEnemyWave() {
    int count = 3 + (waveNumber * 2);
    for (int i = 0; i < count; i++) {
        for (int attempt = 0; attempt < 10; attempt++) {
            int x = rand() % SIZE;
            int y = rand() % SIZE;
            if (rand() % 2 == 0) x = (rand() % 2 == 0) ? 0 : SIZE - 1;
            else y = (rand() % 2 == 0) ? 0 : SIZE - 1;

            if (isValidMove(x, y)) {
                spawnUnit(UnitType::SOLDIER, Position(x, y), 2);
                break;
            }
        }
    }
}

void GameEngine::addBuilding(const Position &pos, const BuildingType &type, int owner) {
    int hp = (type == BuildingType::HUB) ? 1500 : (type == BuildingType::GLORBSTRACTOR ? 800 : 500);
    auto newBuilding = std::make_unique<Building>(pos, type, hp, owner);
    int id = newBuilding->getId();
    buildings.push_back(std::move(newBuilding));
    if (pos.getX() >= 0 && pos.getX() < SIZE && pos.getY() >= 0 && pos.getY() < SIZE) {
        tiles[pos.getX()][pos.getY()] = id;
    }
}

void GameEngine::addResource(const Position &pos, int amount, ResourceType type) {
    if (isValidMove(pos.getX(), pos.getY())) {
        resources.push_back(std::make_unique<Resource>(pos, amount, type));
        tiles[pos.getX()][pos.getY()] = resources.back()->getId();
    }
}

void GameEngine::Update(float deltaTime) {
    float timeScale = 0.75f;
    float scaledDt = deltaTime * timeScale;
    totalGameTime += scaledDt;
    moveTimer += scaledDt;

    if (totalGameTime >= 60.0f * (waveNumber + 1)) {
        spawnEnemyWave();
        waveNumber++;
    }

    for (auto &b: buildings) {
        b->update(scaledDt);
        if (b->isProductionComplete()) {
            spawnUnit(b->popProduction(), findNearestFreeTile(b->getPosition()), b->getOwner());
        }
    }

    if (moveTimer > 0.1f) {
        for (auto &unit: units) {
            if (unit->getHitPoints() > 0) unit->update(this, scaledDt);
            if (unit->getOwner() == 2 && unit->getCurrentState()) {
                if (dynamic_cast<IdleState *>(unit->getCurrentState().get())) {
                    const Building *target = findNearestEntity<Building>(unit->getPosition(), buildings, 1);
                    if (target) {
                        unit->setAttackTarget(target->getPosition());
                        unit->setState(std::make_shared<AttackState>());
                    }
                }
            }
        }
        moveTimer = 0.0f;
    }

    auto it = units.begin();
    while (it != units.end()) {
        if ((*it)->getHitPoints() <= 0) {
            Position p = (*it)->getPosition();
            if (tiles[p.getX()][p.getY()] == (*it)->getId()) tiles[p.getX()][p.getY()] = 0;
            std::erase(selectedUnits, it->get());
            it = units.erase(it);
        } else ++it;
    }

    auto bIt = buildings.begin();
    while (bIt != buildings.end()) {
        if ((*bIt)->getHitPoints() <= 0) {
            Position p = (*bIt)->getPosition();
            if (tiles[p.getX()][p.getY()] == (*bIt)->getId()) tiles[p.getX()][p.getY()] = 0;
            if (selectedBuilding == bIt->get()) selectedBuilding = nullptr;
            bIt = buildings.erase(bIt);
        } else ++bIt;
    }
}

void GameEngine::render(sf::RenderWindow &window) {
    window.setView(gameView);

    sf::RectangleShape tileShape({(float) TILE_SIZE, (float) TILE_SIZE});
    tileShape.setOutlineThickness(1.0f);
    tileShape.setOutlineColor(sf::Color(50, 50, 50));

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            tileShape.setPosition({(float) x * TILE_SIZE, (float) y * TILE_SIZE});
            tileShape.setFillColor(sf::Color(34, 139, 34));
            window.draw(tileShape);
        }
    }

    sf::RectangleShape resShape({(float) TILE_SIZE - 12, (float) TILE_SIZE - 12});
    for (const auto &res: resources) {
        Position pos = res->getPosition();
        resShape.setPosition({(float) pos.getX() * TILE_SIZE + 6, (float) pos.getY() * TILE_SIZE + 6});
        if (res->getResourceType() == ResourceType::GOLD) resShape.setFillColor(sf::Color(255, 215, 0));
        else resShape.setFillColor(sf::Color(138, 43, 226));
        window.draw(resShape);
    }

    sf::RectangleShape bShape({(float) TILE_SIZE - 4, (float) TILE_SIZE - 4});
    for (const auto &b: buildings) {
        bShape.setPosition({
            (float) b->getPosition().getX() * TILE_SIZE + 2, (float) b->getPosition().getY() * TILE_SIZE + 2
        });

        if (b->getType() == BuildingType::HUB) bShape.setFillColor(sf::Color::Cyan);
        else if (b->getType() == BuildingType::BARRACKS) bShape.setFillColor(sf::Color(139, 69, 19));
        else bShape.setFillColor(sf::Color(75, 0, 130));

        if (b->getHitPoints() < 500) {
            bShape.setOutlineThickness(2.0f);
            bShape.setOutlineColor(sf::Color::Red);
        } else {
            bShape.setOutlineThickness(0.0f);
        }
        window.draw(bShape);

        if (b.get() == selectedBuilding) {
            sf::CircleShape sel(TILE_SIZE / 1.5f);
            sel.setOrigin({TILE_SIZE / 1.5f, TILE_SIZE / 1.5f});
            sel.setPosition({
                (float) b->getPosition().getX() * TILE_SIZE + TILE_SIZE / 2,
                (float) b->getPosition().getY() * TILE_SIZE + TILE_SIZE / 2
            });
            sel.setFillColor(sf::Color::Transparent);
            sel.setOutlineColor(sf::Color::Green);
            sel.setOutlineThickness(2);
            window.draw(sel);
        }
    }

    sf::CircleShape uShape(TILE_SIZE / 2.5f);
    for (const auto &u: units) {
        float offset = (TILE_SIZE - (uShape.getRadius() * 2)) / 2;
        uShape.setPosition({
            (float) u->getPosition().getX() * TILE_SIZE + offset, (float) u->getPosition().getY() * TILE_SIZE + offset
        });

        if (u->getOwner() == 1) {
            if (dynamic_cast<Worker *>(u.get())) uShape.setFillColor(sf::Color::Green);
            else uShape.setFillColor(sf::Color(0, 0, 139));
        } else {
            uShape.setFillColor(sf::Color::Red);
        }
        window.draw(uShape);

        if (std::find(selectedUnits.begin(), selectedUnits.end(), u.get()) != selectedUnits.end()) {
            sf::CircleShape sel(TILE_SIZE / 2.5f + 2);
            sel.setPosition({uShape.getPosition().x - 2, uShape.getPosition().y - 2});
            sel.setFillColor(sf::Color::Transparent);
            sel.setOutlineColor(sf::Color::White);
            sel.setOutlineThickness(1);
            window.draw(sel);
        }
    }

    if (isDragging) {
        sf::Vector2f curPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), gameView);
        sf::RectangleShape box;
        box.setPosition(dragStart);
        box.setSize(curPos - dragStart);
        box.setFillColor(sf::Color(0, 255, 0, 50));
        box.setOutlineColor(sf::Color::Green);
        box.setOutlineThickness(1);
        window.draw(box);
    }

    window.setView(uiView);
    sf::RectangleShape uiBg({(float) window.getSize().x, (float) UI_HEIGHT});
    uiBg.setPosition({0.f, static_cast<float>(window.getSize().y - UI_HEIGHT)});
    uiBg.setFillColor(sf::Color(40, 40, 40, 230));
    window.draw(uiBg);

    std::stringstream ss;
    ss << "GOLD: " << playerGold << "  |  GLORBIUM: " << playerGlorbium
            << "  |  WAVE: " << waveNumber << "  |  TIME: " << (int) totalGameTime << "s";

    if (showBuildMenu) {
        ss << "\n[BUILD MENU]:  (B) Barracks   (H) Hub   (G) Glorbstractor";
        resourceText.setFillColor(sf::Color::Yellow);
    } else if (isPlacing) {
        ss << "\nPLACING... (Left Click to Build, Right Click Cancel)";
        resourceText.setFillColor(sf::Color::Cyan);
    } else {
        ss << "\nControls: [B] Build  [A] Attack  [Rt-Click] Move/Gather  [Q] Train";
        resourceText.setFillColor(sf::Color::White);
    }

    resourceText.setString(ss.str());
    resourceText.setPosition({20.f, static_cast<float>(window.getSize().y - UI_HEIGHT + 10.f)});
    window.draw(resourceText);
}


Position GameEngine::findNearestFreeTile(Position center) {
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            int nx = center.getX() + x;
            int ny = center.getY() + y;
            if (isValidMove(nx, ny)) return Position(nx, ny);
        }
    }
    return Position(-1, -1);
}

void GameEngine::moveUnit(Unit *unit, const Position &new_position) {
    tiles[unit->getPosition().getX()][unit->getPosition().getY()] = 0;
    tiles[new_position.getX()][new_position.getY()] = unit->getId();
    *unit = new_position;
}

std::vector<Position> GameEngine::findPath(Position start, Position target) {
    std::vector<Position> path;
    if (target.getX() < 0 || target.getX() >= SIZE || target.getY() < 0 || target.getY() >= SIZE) return path;
    if (start == target) return path;

    std::queue<Position> frontier;
    frontier.push(start);
    std::map<Position, Position> came_from;
    came_from[start] = start;
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
            if (next.getX() < 0 || next.getX() >= SIZE || next.getY() < 0 || next.getY() >= SIZE) continue;
            if ((tiles[next.getX()][next.getY()] == 0 || next == target) && came_from.find(next) == came_from.end()) {
                frontier.push(next);
                came_from[next] = current;
            }
        }
    }
    if (found) {
        Position step = target;
        while (!(step == start)) {
            path.push_back(step);
            step = came_from[step];
        }
        std::reverse(path.begin(), path.end());
    }
    return path;
}

void GameEngine::setUnitPath(Unit *unit, const Position &target, bool stopAdjacent) {
    std::vector<Position> path = findPath(unit->getPosition(), target);
    if (path.empty() && !(unit->getPosition() == target)) return;
    if (!path.empty() && stopAdjacent && path.back() == target) path.pop_back();
    unit->setPath(path);
}

bool GameEngine::isValidMove(int x, int y) const {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return false;
    return tiles[x][y] == 0;
}

void GameEngine::resolveCombat(const Unit *attacker, const Position &targetPos) {
    int id = tiles[targetPos.getX()][targetPos.getY()];
    for (auto &u: units)
        if (u->getId() == id) {
            u->takeDamage(attacker->getAttackDamage());
            return;
        }
    for (auto &b: buildings)
        if (b->getId() == id) {
            b->takeDamage(attacker->getAttackDamage());
            return;
        }
}

Position GameEngine::findNearestBuilding(Position start, BuildingType type, int owner) {
    const Building *b = findNearestEntity<Building>(start, buildings, owner);
    if (b && b->getType() == type) return b->getPosition();
    return Position(-1, -1);
}

std::pair<int, ResourceType> GameEngine::harvestAt(Position pos, int amount) {
    int tileId = tiles[pos.getX()][pos.getY()];
    for (auto it = resources.begin(); it != resources.end(); ++it) {
        if ((*it)->getId() == tileId) {
            int mined = (*it)->harvest(amount);
            ResourceType type = (*it)->getResourceType();
            if ((*it)->isDepleted()) {
                tiles[pos.getX()][pos.getY()] = 0;
                resources.erase(it);
            }
            return {mined, type};
        }
    }
    for (auto &b: buildings) {
        if (b->getId() == tileId && b->getType() == BuildingType::GLORBSTRACTOR) {
            return {b->harvest(amount), ResourceType::GLORBIUM};
        }
    }
    return {0, ResourceType::GOLD};
}

void GameEngine::depositGlobalResources(int amount, ResourceType type) {
    if (type == ResourceType::GOLD) playerGold += amount;
    else if (type == ResourceType::GLORBIUM) playerGlorbium += amount;
}

void GameEngine::handleInput(const std::optional<sf::Event> &event, sf::RenderWindow &window) {
    if (!event) return;


    if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            isPlacing = false;
            showBuildMenu = false;
            isAttackMode = false;
            selectedUnits.clear();
            selectedBuilding = nullptr;
        }


        if (key->code == sf::Keyboard::Key::A && !selectedUnits.empty()) {
            isAttackMode = !isAttackMode;
            isPlacing = false;
            showBuildMenu = false;
        }


        if (selectedBuilding && key->code == sf::Keyboard::Key::Q) {
            if (selectedBuilding->getType() == BuildingType::HUB && playerGold >= 50) {
                playerGold -= 50;
                selectedBuilding->queueUnit(UnitType::WORKER);
            } else if (selectedBuilding->getType() == BuildingType::BARRACKS && playerGold >= 100) {
                playerGold -= 100;
                selectedBuilding->queueUnit(UnitType::SOLDIER);
            }
        }


        bool hasBuilder = false;
        for (const auto *u: selectedUnits) {
            if (u->canBuild()) {
                hasBuilder = true;
                break;
            }
        }


        if (hasBuilder) {
            if (key->code == sf::Keyboard::Key::B && !showBuildMenu) {
                showBuildMenu = true;
            } else if (showBuildMenu) {
                if (key->code == sf::Keyboard::Key::B) {
                    placementType = BuildingType::BARRACKS;
                    isPlacing = true;
                } else if (key->code == sf::Keyboard::Key::H) {
                    placementType = BuildingType::HUB;
                    isPlacing = true;
                } else if (key->code == sf::Keyboard::Key::G) {
                    placementType = BuildingType::GLORBSTRACTOR;
                    isPlacing = true;
                }
                showBuildMenu = false;
            }
        }
    }


    if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
        window.setView(gameView);
        sf::Vector2f worldPos = window.mapPixelToCoords(mouse->position, gameView);
        int gx = static_cast<int>(worldPos.x / TILE_SIZE);
        int gy = static_cast<int>(worldPos.y / TILE_SIZE);
        Position clickPos(gx, gy);


        if (mouse->button == sf::Mouse::Button::Left) {
            if (isPlacing) {
                Worker *builder = nullptr;
                for (auto *u: selectedUnits) {
                    if (auto *w = dynamic_cast<Worker *>(u)) {
                        builder = w;
                        break;
                    }
                }

                if (builder) {
                    bool canBuildHere = true;
                    if (placementType == BuildingType::GLORBSTRACTOR) {
                        bool onGeyser = false;
                        for (const auto &r: resources) {
                            if (r->getPosition() == clickPos && r->getResourceType() == ResourceType::GLORBIUM)
                                onGeyser = true;
                        }
                        if (!onGeyser) canBuildHere = false;
                    } else {
                        if (!isValidMove(gx, gy)) canBuildHere = false;
                    }

                    int cost = (placementType == BuildingType::HUB)
                                   ? 1000
                                   : (placementType == BuildingType::GLORBSTRACTOR ? 150 : 200);


                    if (canBuildHere && playerGold >= cost) {
                        playerGold -= cost;
                        builder->setBuildingProject(placementType);
                        builder->setBuildTarget(clickPos);
                        builder->setState(std::make_shared<BuildState>());


                        isPlacing = false;
                    }
                }
            } else if (isAttackMode) {
                for (auto *u: selectedUnits) {
                    u->setAttackTarget(clickPos);
                    u->setState(std::make_shared<AttackState>());
                }
                isAttackMode = false;
            } else {
                isDragging = true;
                dragStart = worldPos;
            }
        } else if (mouse->button == sf::Mouse::Button::Right) {
            if (isPlacing) {
                isPlacing = false;
                return;
            }

            int targetId = tiles[gx][gy];
            bool handled = false;


            if (targetId != 0) {
                for (const auto &r: resources) {
                    if (r->getId() == targetId) {
                        for (auto *u: selectedUnits) {
                            if (u->canHarvest()) {
                                setUnitPath(u, clickPos, true);
                                if (auto *w = dynamic_cast<Worker *>(u)) {
                                    w->setResourceTarget(clickPos);
                                    w->setState(std::make_shared<GatherState>());
                                }
                            }
                        }
                        handled = true;
                        break;
                    }
                }
            }


            if (!handled) {
                for (auto *u: selectedUnits) {
                    if (isValidMove(gx, gy)) {
                        setUnitPath(u, clickPos, false);
                        u->setState(std::make_shared<MovingState>());
                    }
                }
            }
        }
    }


    if (const auto *mouseRel = event->getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseRel->button == sf::Mouse::Button::Left && isDragging) {
            isDragging = false;
            sf::Vector2f dragEndPos = window.mapPixelToCoords(mouseRel->position, gameView);


            float left = std::min(dragStart.x, dragEndPos.x);
            float top = std::min(dragStart.y, dragEndPos.y);
            float width = std::abs(dragStart.x - dragEndPos.x);
            float height = std::abs(dragStart.y - dragEndPos.y);
            sf::FloatRect selectionRect({left, top}, {width, height});

            selectedUnits.clear();
            selectedBuilding = nullptr;


            if (selectionRect.size.x < 5.0f && selectionRect.size.y < 5.0f) {
                for (auto &b: buildings) {
                    sf::FloatRect bBounds(
                        {(float) b->getPosition().getX() * TILE_SIZE, (float) b->getPosition().getY() * TILE_SIZE},
                        {(float) TILE_SIZE, (float) TILE_SIZE}
                    );
                    if (bBounds.contains(dragEndPos) && b->getOwner() == 1) {
                        selectedBuilding = b.get();
                        return;
                    }
                }
            }
            for (auto &u: units) {
                if (u->getOwner() == 1) {
                    sf::Vector2f uPos(
                        (float) u->getPosition().getX() * TILE_SIZE + TILE_SIZE / 2,
                        (float) u->getPosition().getY() * TILE_SIZE + TILE_SIZE / 2
                    );
                    if (selectionRect.contains(uPos)) {
                        selectedUnits.push_back(u.get());
                    }
                }
            }
        }
    }
}
