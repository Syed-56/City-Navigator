#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

// sampleCity.h
class SampleCity {
    private:
    sf::RectangleShape closeButton;
    sf::Font font;
    sf::Text closeText;

    public:
        struct Location { std::string name; float x, y; std::vector<int> neighbors; };
        std::vector<Location> locations;
    
        SampleCity(); // constructor to initialize city
        void draw(sf::RenderWindow &window); // render the city
        void handleEvent(sf::Event &event, bool &returnToMenu);
};    