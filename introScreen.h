#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

// Forward declaration
class SampleCity;

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

    // State management
    bool transitionActive;
    bool navigatingCity;
    sf::Clock transitionClock;
    SampleCity* city;

public:
    IntroScreen(sf::RenderWindow& window);
    ~IntroScreen();
    
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void update(sf::RenderWindow& window); // Add update method
    void draw(sf::RenderWindow& window);
    bool isNavigatingCity() const { return navigatingCity; }
    SampleCity* getCity() const { return city; }
};