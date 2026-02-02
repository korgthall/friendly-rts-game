#include <SFML/Graphics.hpp>
#include "headers/GameEngine.h"
#include "headers/Soldier.h"
#include "headers/Worker.h"
#include "headers/Building.h"

int main() {
    {
        Soldier dummyS(Position(0, 0), 1);
        dummyS.attack();

        Worker dummyW(Position(0, 0), 100, 5, 1, 5, 1, 1, 50, 0, 10);
        dummyW.attack();

        Building dummyB(Position(0, 0), BuildingType::HUB, 100, 1);
        (void) dummyB.getQueueSize();
        dummyB.setStoredResources(0);
    }

    sf::RenderWindow window(sf::VideoMode({1280, 720}), "The Glorbz");
    window.setFramerateLimit(60);

    GameEngine *game = GameEngine::getInstance();
    game->StartGame(window);

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            game->handleInput(event, window);
        }

        game->Update(deltaTime);
        window.clear();
        game->render(window);
        window.display();
    }
    GameEngine::cleanup();
    return 0;
}
