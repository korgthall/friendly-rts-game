#include <SFML/Graphics.hpp>
#include "GameEngine.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "The Glorbz");
    window.setFramerateLimit(60);

    GameEngine* game = GameEngine::getInstance();

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
    Worker* worker = dynamic_cast<Worker*>(game->getUnit().get());
        worker->attack();
    std::cout << game->getPlayerGold() << std::endl;
    GameEngine::cleanup();
    return 0;
}