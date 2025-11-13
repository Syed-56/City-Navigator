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

    bool showPopup;
    sf::RectangleShape popupRect;
    sf::RectangleShape closeButtonRect;
    sf::Text closeButtonText;

    sf::RectangleShape createCityButtonRect;
    sf::Text createCityButtonText;

    sf::RectangleShape navigateCityButtonRect;
    sf::Text navigateCityButtonText;

public:
    IntroScreen(sf::RenderWindow& window);
    void show(sf::RenderWindow& window);
};
