#include "sampleCity.h"
#include <SFML/Graphics.hpp>

SampleCity::SampleCity() {
     // Load font
    font.loadFromFile("arial.ttf");

     // Button box
    closeButton.setSize(sf::Vector2f(40, 40));
    closeButton.setPosition(10, 10);   // top-left
    closeButton.setFillColor(sf::Color(200, 50, 50)); // red
 
    // X text
    closeText.setFont(font);
    closeText.setString("X");
    closeText.setCharacterSize(28);
    closeText.setFillColor(sf::Color::White);
    closeText.setPosition(20, 12);

    // Define some sample locations
    Location park{"Park", 100, 100, {1, 2}};
    Location school{"School", 300, 100, {0, 2}};
    Location market{"Market", 200, 300, {0, 1}};
    Location hospital{"Hospital", 500, 200, {}}; // isolated for now
    Location library{"Library", 400, 400, {2, 3}};

    // Add locations to vector
    locations.push_back(park);
    locations.push_back(school);
    locations.push_back(market);
    locations.push_back(hospital);
    locations.push_back(library);

    // Add additional road connections (if needed)
    locations[3].neighbors.push_back(4); // hospital connected to library
    locations[4].neighbors.push_back(3); // library connected to hospital
}

void SampleCity::handleEvent(sf::Event &event, bool &returnToMenu) {
    if(event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
        if(closeButton.getGlobalBounds().contains(mouse))
            returnToMenu = false; // <-- X button returns to intro screen
    }
}

void SampleCity::draw(sf::RenderWindow &window) {
    sf::Color lightGreen(170, 255, 170);
    window.clear(lightGreen);
    window.draw(closeButton);
    window.draw(closeText);
    // Draw roads
    for (size_t i=0;i<locations.size();i++) {
        for(int neighbor: locations[i].neighbors){
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(locations[i].x, locations[i].y), sf::Color::Black),
                sf::Vertex(sf::Vector2f(locations[neighbor].x, locations[neighbor].y), sf::Color::Black)
            };
            window.draw(line, 2, sf::Lines);
        }
    }
    

    // Draw locations
    for(auto &loc: locations){
        sf::CircleShape circle(10);
        circle.setFillColor(sf::Color::White);
        circle.setPosition(loc.x-10, loc.y-10);
        window.draw(circle);
    }
}