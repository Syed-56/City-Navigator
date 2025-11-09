#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

class IntroScreen {
private:
    sf::Font font;
    sf::Text title;

    sf::RectangleShape startButtonRect;
    sf::Text startButtonText;

    sf::RectangleShape exitButtonRect;
    sf::Text exitButtonText;

    sf::Texture backgroundTexture;
    sf::Sprite background;

public:
    IntroScreen(sf::RenderWindow& window);
    void show(sf::RenderWindow& window);
};
