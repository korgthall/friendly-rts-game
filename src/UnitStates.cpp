#include "../headers/UnitStates.h"
#include "../headers/Unit.h"
#include "../headers/Worker.h"
#include "../headers/GameEngine.h"
#include "../headers/Resource.h"
#include "../headers/Building.h"
#include <iostream>
#include <cmath>

void IdleState::update([[maybe_unused]] Unit *unit, [[maybe_unused]] GameEngine *game,
                       [[maybe_unused]] float deltaTime) {
}

void MovingState::update(Unit *unit, GameEngine *game, [[maybe_unused]] float deltaTime) {
    if (unit->hasPath()) {
        Position nextStep = unit->getNextStep();
        if (game->isValidMove(nextStep.getX(), nextStep.getY())) {
            game->moveUnit(unit, nextStep);
            unit->advancePath();
        } else {
            unit->setPath({});
            unit->setState(std::make_shared<IdleState>());
        }
    } else {
        unit->setState(std::make_shared<IdleState>());
    }
}

void AttackState::update(Unit *unit, GameEngine *game, [[maybe_unused]] float deltaTime) {
    Position targetPos = unit->getAttackTarget();
    Position currentPos = unit->getPosition();

    if (std::abs(currentPos.getX() - targetPos.getX()) <= 1 &&
        std::abs(currentPos.getY() - targetPos.getY()) <= 1) {
        game->resolveCombat(unit, targetPos);
        if (game->isValidMove(targetPos.getX(), targetPos.getY())) {
            unit->setState(std::make_shared<IdleState>());
        }
    } else {
        if (!unit->hasPath()) game->setUnitPath(unit, targetPos, true);
        if (unit->hasPath()) {
            Position next = unit->getNextStep();
            if (game->isValidMove(next.getX(), next.getY())) {
                game->moveUnit(unit, next);
                unit->advancePath();
            }
        }
    }
}

void GatherState::update(Unit *unit, GameEngine *game, [[maybe_unused]] float deltaTime) {
    Worker *w = dynamic_cast<Worker *>(unit);
    if (!w) {
        unit->setState(std::make_shared<IdleState>());
        return;
    }

    Position current = w->getPosition();
    Position target = w->getResourceTarget();
    int dist = std::abs(current.getX() - target.getX()) + std::abs(current.getY() - target.getY());

    if (w->isFull()) {
        w->setState(std::make_shared<DepositState>());
        return;
    }

    if (dist <= 1) {
        std::pair<int, ResourceType> result = game->harvestAt(target, w->getGatherSpeed());
        if (result.first > 0) w->addResource(result.first, result.second);
    } else {
        if (!w->hasPath()) game->setUnitPath(w, target, true);
        if (w->hasPath()) {
            Position next = w->getNextStep();
            if (game->isValidMove(next.getX(), next.getY())) {
                game->moveUnit(w, next);
                w->advancePath();
            } else w->setPath({});
        }
    }
}

void DepositState::update(Unit *unit, GameEngine *game, [[maybe_unused]] float deltaTime) {
    Worker *w = dynamic_cast<Worker *>(unit);
    if (!w) return;

    Position current = w->getPosition();
    const Building *hub = game->findNearestEntity<Building>(current, game->getBuildings(), w->getOwner());

    if (!hub || hub->getType() != BuildingType::HUB) {
        unit->setState(std::make_shared<IdleState>());
        return;
    }

    Position hubPos = hub->getPosition();

    if (std::abs(current.getX() - hubPos.getX()) > 1 || std::abs(current.getY() - hubPos.getY()) > 1) {
        if (!w->hasPath() || w->getNextStep() == current) game->setUnitPath(w, hubPos, true);
        if (w->hasPath()) {
            Position next = w->getNextStep();
            if (game->isValidMove(next.getX(), next.getY())) {
                game->moveUnit(w, next);
                w->advancePath();
            }
        }
    } else {
        game->depositGlobalResources(w->dumpResources(), w->getCarriedType());
        w->setState(std::make_shared<GatherState>());
    }
}

void BuildState::update(Unit *unit, GameEngine *game, [[maybe_unused]] float deltaTime) {
    Worker *w = dynamic_cast<Worker *>(unit);
    if (!w) {
        unit->setState(std::make_shared<IdleState>());
        return;
    }

    Position current = w->getPosition();
    Position target = w->getBuildTarget();
    int dist = std::abs(current.getX() - target.getX()) + std::abs(current.getY() - target.getY());

    if (dist <= 1) {
        w->setPath({});
        game->addBuilding(target, w->getBuildingProject(), w->getOwner());
        w->setState(std::make_shared<IdleState>());
    } else {
        if (!w->hasPath()) game->setUnitPath(w, target, true);
        if (w->hasPath()) {
            Position next = w->getNextStep();
            if (game->isValidMove(next.getX(), next.getY())) {
                game->moveUnit(w, next);
                w->advancePath();
            } else w->setPath({});
        }
    }
}
